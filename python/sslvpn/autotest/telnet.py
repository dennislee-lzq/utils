#!/usr/bin/python
# coding=utf8
#
# Copyright(C) 2015 Ruijie Network. All rights reserved.
# Created on 2015.12.18
# Author: lizhiqiang@ruijie.com.cn
# Description:
#     Implement telnet tool.
#
# History: 
#  V1.0 lizhiqiang@ruijie.com.cn 2015.12.18
#       Create 

import telnetlib
import os, sys, time
import string
import re
from sre_compile import isstring
import log

class RgTelnet(telnetlib.Telnet):
    def __init__(self, host=None, port=23, username="", password="", timeout=3, debug_on=False, prompt='(#|>)', prompt_is_regexp=True):
        '''
        host: 主机IP
        port: telnet 端口号
        username: 用户名
        password: 密码
        debug_on: debug是否开启
        prompt: 提示，如Ruijie#中的#
        prompt_is_regexp：提示是否为正则表达式
        '''
        telnetlib.Telnet.__init__(self, host, int(port) if port else 23)
        self.debug_on = debug_on
        self.prompt = prompt
        self._set_newline('\r\n')
        self.timeout = timeout
        self._set_prompt(prompt, prompt_is_regexp)
        self.username = username
        self.password = password

    def _verify_connection(self):
        '''
        判断telnet是否连接
    
        '''
        ""
        if not self.sock:
            raise RuntimeError('No connection open')

    def _read_until(self, expected):
        '''
         读消息，直到期盼字符串出现，有两个返回值
        '''         
        self._verify_connection()
        output = telnetlib.Telnet.read_until(self, expected, self.timeout)
        return output.endswith(expected), output
    
    def _read_until_regexp(self, *expected):
        '''
        读消息，直到期盼字符串出现，期盼的字符串是一个正则表达式，有两个返回值
        '''
        self._verify_connection()
        expected = [self._encode(exp) if isinstance(exp, unicode) else exp
                    for exp in expected]
        return self._telnet_read_until_regexp(expected)

    def _telnet_read_until_regexp(self, expected_list):
        try:
            index, _, output = self.expect(expected_list, self.timeout)
        except TypeError:
            index, output = -1, ''
        return index != -1, output
    
    def _read_until_prompt(self):
        prompt, regexp = self.prompt
        read_until = self._read_until_regexp if regexp else self._read_until
        return read_until(prompt)
    
    def _set_newline(self, newline):
        self._newline = str(newline).upper().replace('LF','\n').replace('CR','\r')
    

    def _set_prompt(self, prompt, prompt_is_regexp):
        if prompt_is_regexp:
            self.prompt = (re.compile(prompt), True)
        else:
            self.prompt = (prompt, False)
            
    def _prompt_is_set(self):
        return self.prompt[0] is not None
    
    def _submit_credentials(self, username, password, login_prompt, password_prompt):
        """
        输入用户名和密码：
        username: 用户名
        password: 密码
        login_prompt: 输入用户名登录提示符
        password_prompt: 输入密码提示符
        """
        try: 
            output = self.read_until(login_prompt, self.timeout)
            self.rg_debug('try to get login prompt\n' + output);
            self.write_bare(username + self._newline)
            output += self.read_until(password_prompt, self.timeout)
            self.rg_debug('try to get password prompt\n' + output);
            self.write_bare(password + self._newline)
            return output
        except:
            self.rg_debug('failed to input credentials\n');
            return ""
    
    def _submit_enable(self, enable_cli, password, password_prompt):
        try: 
            self.write_bare(enable_cli + self._newline)
            output = self.read_until(password_prompt, self.timeout)
            self.rg_debug('try to get password prompt\n' + output);
            self.write_bare(password + self._newline)
            return output
        except:
            self.rg_debug('failed to input credentials\n');
            return ""

    def write_bare(self, text):
        """Writes the given text, and nothing else, into the connection.
        This keyword does not append a newline nor consume the written text.
        Use `Write` if these features are needed.
        """
        self._verify_connection()
        telnetlib.Telnet.write(self, text)

    def set_prompt(self, prompt, prompt_is_regexp=False):
        """Sets the prompt used by `Read Until Prompt` and `Login` in the current connection.

        If `prompt_is_regexp` is given any true value, including any non-empty
        string, the given `prompt` is considered to be a regular expression.

        The old prompt is returned and can be used to restore the prompt later.
        """
        self._verify_connection()
        old = self.prompt
        self._set_prompt(prompt, prompt_is_regexp)
        if old[1]:
            return old[0].pattern, True
            
        self.prompt = prompt
        self.prompt_is_regexp = prompt_is_regexp
        
        return old
    
    def rg_debug(self, output):
        if self.debug_on:
             print output

    def rg_send_cmd(self, cmd):
        self._verify_connection()
        self.write(cmd + "\n")

    def rg_read_all(self, timeout=0.01, max_timeout=3, wait_time=1):
        if not self._prompt_is_set():
            raise RuntimeError('Prompt is not set.')
        
        time.sleep(wait_time)
        self._verify_connection()
        output = self.read_very_eager() #read all data from socket buffer
        self.rg_debug("---first read all---\n" + output)
        
        maxtime = time.time() + max_timeout
        while output == '': #try to read the data
            time.sleep(0.1)
            self._verify_connection()
            output += self.read_very_eager()
            self.rg_debug('---no data, wait for data---\n' + output);
            if time.time() > maxtime:
                break;
            if output == '':
                self.rg_debug('---still no data, return---\n' + output);
                return output

        old_timeout = self.timeout; #get the old timeout, after finish set timeout back to the old one
        self.timeout = timeout
        success, output_tmp = self._read_until_prompt() #try to read until '#'
        output += output_tmp
        flag = 0

        maxtime = time.time() + max_timeout
        if success: #success read the '#', then try to read all the data until cannot read the '#'
            self.rg_debug('---still have data, read again---\r\n' + output);
            while success:
                success, output_tmp = self._read_until_prompt()
                output += output_tmp
        else: #fail read the '#', then try to write a SPACE and then read until '#', stop when success read the '#'
            self.rg_debug('---perhaps have --More-- ---\r\n' + output);
            if '--More--' not in output and '#' in output:
                self._verify_connection()
                self.timeout = old_timeout
                self.rg_debug('---without --More--, and reach the #, return---\r\n' + output);
                return output
            flag = 1
            self.write_bare(' ')
            success, output_tmp = self._read_until_prompt()
            output += output_tmp
            while not success:
                self.write_bare(' ')
                success, output_tmp = self._read_until_prompt()
                output += output_tmp
                if time.time() > maxtime:
                    break
        self._verify_connection()
        self.timeout = old_timeout
        if '--More--' not in output and flag == 1:
            self.rg_debug('---without --More--, but enter a lot of space, need clear the space---\r\n' + output);
            self.write_bare(self._newline)
            success, output_tmp = self._read_until_prompt()
        
        self.rg_debug('---the end---\r\n' + output);
        output = string.replace(output, " --More--           ", "")
        return output
    
    def login(self, username, password, login_prompt='Username:',
              password_prompt='Password:', login_timeout=1,
              login_incorrect='Login incorrect'):
        if not self._prompt_is_set():
            raise RuntimeError('Prompt is not set.')
        
        output = self._submit_credentials(username, password, login_prompt,
                                          password_prompt)
        success, output2 = self._read_until_prompt()
        output += output2
        self.rg_debug(output)
        if not success:
            raise AssertionError('Login incorrect')
        return output
    
    def enable(self, enable_cli, password, 
              password_prompt='Password:', login_timeout=1,
              login_incorrect='Login incorrect'):
         
        if not self._prompt_is_set():
            raise RuntimeError('Prompt is not set.')
         
        output = self._submit_enable(enable_cli, password, password_prompt)
        success, output2 = self._read_until_prompt()
        output += output2
        self.rg_debug(output)
        if not success:
            raise AssertionError('Login incorrect')
        return output
    
    # 内置功能函数
    def rg_findall(self, pattern, config):
        ret = re.findall(pattern, config)
        if ret == []:
            return ret
        
        if type(ret[0]).__name__ == 'tuple':
            result = []
            for tmp in ret:
                 result.append(tmp[0])
            return result
        else:
            return ret

    def rg_show(self, command, timeout=0.1, max_timeout=10, wait_time=1.5):
        try:
            self.rg_send_cmd(" " + command)
            return self.rg_read_all(timeout, max_timeout, wait_time)
        except:
            self.rg_debug("failed to show: {0}".format(command));
            return ""
    
    def rg_sys_shell(self, command, timeout=1, max_timeout=10, wait_time=1):
        try:
            self.rg_send_cmd(" enable")
            self.rg_send_cmd(" run-system-shell")
            self.rg_read_all(timeout, max_timeout, wait_time)
            self.rg_send_cmd(command)
            temp = self.rg_read_all(timeout, max_timeout, wait_time)
            self.rg_send_cmd("exit")
            return temp
        except:
            self.rg_debug("failed to Linux shell: {0}".format(command));
            return ""

    def rg_mac_table(self):
        table = []
        mac = self.rg_show("show mac", wait_time=0.1)
        tmp = re.findall(r"(\d{1,4})\s+([0-9A-Fa-f]{4}\.[0-9A-Fa-f]{4}\.[0-9A-Fa-f]{4}).+?(\w+)\s+(\w+ [A-Za-z0-9/]+)", mac)
        for vlan, mac, type, interface in tmp:
            table.append({"vlan": vlan, "type": type, "interface": interface})
        
        return table
    
    def rg_arp_table(self):
        table = []
        arp = self.rg_show("show arp detail", wait_time=0.1)
        tmp = re.findall(r"(\d+\.\d+\.\d+\.\d+)\s+([0-9A-Fa-f]{4}\.[0-9A-Fa-f]{4}\.[0-9A-Fa-f]{4})\s+(\w+)\s+([0-9-]+)\s+([A-Za-z0-9/]+)\s+([A-Za-z0-9/-]+)", arp)
        for ip, mac, type, age, interface, port in tmp:
            table.append({"ip": ip, "mac": mac, "type": type, "age": age, "interface": interface, "port": port})
        
        return table
    
    def rg_tcp_connect_table(self, status=""):
        table = []
        if status == "":
            show = self.rg_show("show tcp connect", wait_time=0.1)
        else:
            show = self.rg_show("show tcp connect | include {0}".format(status), wait_time=0.1)
        tmp = re.findall(r"(\d+\.\d+\.\d+\.\d+\.\d+)\s+([0-9]{1,3}\.\d+\.\d+\.\d+\.\d+)\s+(\w+)", show)
        for local, foreign, state in tmp:
            table.append({"local": local, "foreign": foreign, "states": state})
        return table
    
    def rg_cpu_total(self):
        dict = {}
        show = self.rg_show("show cpu | include CPU", wait_time=0.1)
        tmp = re.findall(r"in\s+(\w+\s+\w+)\s*:\s*(\d+\.\d+%)", show)
        for key,value in tmp:
            dict[key] = value;
    
        return dict
    
    def rg_cpu_table(self):
        table = []
        show = self.rg_show("show cpu", wait_time=0.5)
        tmp = re.findall(r"(\d+)\s+(\d+\.\d+%)\s+(\d+\.\d+%)\s+(\d+\.\d+%)\s+(.+?)\s*\r?\n", show)
        for no,five_sec,one_min,five_min,process in tmp:
            table.append({"no": no, "5sec": five_sec, "1min": one_min, "5min": five_min, "process": process})
        return table

def telnet_test():
    host="192.168.197.113"
    port=23
    username="root"
    password=""
    timeout=3
    debug_on=False;
    prompt="(#|>|# )"
    prompt_is_regexp=True

    vty=RgTelnet(host, port, username, password, timeout, debug_on, prompt, prompt_is_regexp);
    vty.login("root", "");
    vty.rg_send_cmd("show sslvpn gateway | inc Service");
    out=vty.rg_read_all();
    print out

if __name__ == '__main__':
    log.log_init();
    telnet_test