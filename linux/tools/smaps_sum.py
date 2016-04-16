#!/usr/bin/env python
#-*- encoding:cp936 -*-

import getopt
import re
import sys

#Empty space found from 0x0001ff64 to 0x00020000
ignore_str1 = r''

ignore_strs = []
#ignore_strs.append(re.compile(ignore_str1))

#08048000-08090000 r-xp 00000000 08:01 1050262    /usr/bin/nm-applet
#00400000-00405000 r-xp 00000000 1f:03 295        /sbin/rg_wvlan.elf
file_str = r'.*?\-.*?([rwxp\-]+).*?:.*? \d+ *(.*)';
file_re = re.compile(file_str)

#Private_Clean:        76 kB
#Private_Dirty:         0 kB
frag_str1 = r'Private_Clean: *(\d+)';
frag_str2 = r'Private_Dirty: *(\d+)';
frag_res = []
frag_res.append(re.compile(frag_str1))
frag_res.append(re.compile(frag_str2))

#Pss:                   0 kB
pss_re = re.compile(r'Pss: *(\d+)')

#Help string
help_str = 'usage: %s [-c compile.txt] [-w warning.txt] [-n nosame.txt] [-i in.txt] [-o out.txt] <in.txt >out.txt'
def usage(program_name):
  print help_str % program_name

def main():
  '''filter smaps.log from sys.stdin to sys.stdout'''

  try:
    opts, args = getopt.getopt(sys.argv[1:], 'c:a:e:w:i:o:')
  except getopt.GetoptError, err:
    # print help information and exit
    print str(err) # will print something like "option -a not recognized"
    usage(sys.argv[0])
    sys.exit(-1)

  inputfilename, outputfilename = "stdin", "stdout"
  compilefile, ewallfile, errorfile, warningfile = None, None, None, None
  inputfile, outputfile = None, None
  for opt, arg in opts:
    if opt == '-c':
      compilefile = file(arg, 'w')
    elif opt == '-a':
      ewallfile = file(arg, 'w')
    elif opt == '-e':
      errorfile = file(arg, 'w')
    elif opt == '-w':
      warningfile = file(arg, 'w')
    elif opt == '-i':
      inputfilename = arg
      inputfile = file(arg, 'r')
    elif opt == '-o':
      outputfilename = arg
      outputfile = file(arg, 'w')
    else:
      assert False, "unhandled option"

  if not inputfile:
    inputfile = sys.stdin               # default: from cmd
  if not outputfile:
    outputfile = sys.stdout             # default: to cmd

  jffs2log, jffs2log_total = {}, {}
  line = inputfile.readline()
  while line:
    line = line.rstrip()

    if compilefile:
      print >>compilefile, line         # compile.txt
      #compilefile.flush()

    process = False
    for ignore_re in ignore_strs:
      if ignore_re.match(line):
        process = True
        break

    if not process:
      m = file_re.match(line)
      if m:
        # add
        #print 1, m.groups()
        file_attr_tmp = m.group(1)
        file_name_tmp = m.group(2)
        #if not file_name_tmp == '':
        #    file_attr = file_attr_tmp
        #    file_name = file_name_tmp
        #elif file_name.find('/dev/shm/') >= 0:
        #    file_attr = file_attr_tmp
        #    file_name = file_name_tmp
        #elif not file_attr_tmp == file_attr:
        #    file_attr = file_attr_tmp
        #    file_name = file_name_tmp
        file_attr = None #file_attr_tmp
        file_name = file_name_tmp

        if file_name.find('/sbin/') >= 0 or file_name.find('/bin/') >= 0:    # /sbin/apag.elf or /bin/busybox
          log_name = file_name
          if not jffs2log.has_key(log_name):
            jffs2log[log_name] = {}
            jffs2log_total[log_name] = {}
        if not jffs2log[log_name].has_key((file_name, file_attr)):
          jffs2log[log_name][(file_name, file_attr)] = {}
        if not jffs2log[log_name][(file_name, file_attr)].has_key('private'):
          jffs2log[log_name][(file_name, file_attr)]['private'] = 0
        if not jffs2log[log_name][(file_name, file_attr)].has_key('pss'):
          jffs2log[log_name][(file_name, file_attr)]['pss'] = 0
        if not jffs2log_total[log_name].has_key('private'):
          jffs2log_total[log_name]['private'] = 0
        if not jffs2log_total[log_name].has_key('pss'):
          jffs2log_total[log_name]['pss'] = 0
        process = True

    if not process:
      for frag_re in frag_res:
        m = frag_re.match(line)
        if m:
          # compute
          #print 2, m.groups()
          pri_size = int(m.group(1))
          pri_sizes = jffs2log[log_name][(file_name, file_attr)]['private']
          jffs2log[log_name][(file_name, file_attr)]['private'] = pri_sizes + pri_size
          pri_sizes = jffs2log_total[log_name]['private']
          jffs2log_total[log_name]['private'] = pri_sizes + pri_size
          process = True
          break

    if not process:
      m = pss_re.match(line)
      if m:
        # compute
        #print 3, m.groups()
        pss_size = int(m.group(1))
        pss_sizes = jffs2log[log_name][(file_name, file_attr)]['pss']
        jffs2log[log_name][(file_name, file_attr)]['pss'] = pss_sizes + pss_size
        pss_sizes = jffs2log_total[log_name]['pss']
        jffs2log_total[log_name]['pss'] = pss_sizes + pss_size
        process = True

    line = inputfile.readline()

  total_size = 0
  for key in jffs2log_total:
    log_name = key
    print >>outputfile, log_name, "Total", jffs2log_total[log_name]['private'], jffs2log_total[log_name]['pss']

    for key2 in jffs2log[log_name]:
      file_name, file_attr = key2
      pri_sizes = jffs2log[log_name][key2]['private']
      pss_sizes = jffs2log[log_name][key2]['pss']
      if len(file_name) == 0:
        print >>outputfile, log_name, "NA", pri_sizes, pss_sizes
      else:
        file_name = file_name.replace('(deleted)','')
        print >>outputfile, log_name, file_name, pri_sizes, pss_sizes

  if compilefile: compilefile.close()
  if ewallfile: ewallfile.close()
  if errorfile: errorfile.close()
  if warningfile: warningfile.close()
  if inputfile != sys.stdin: inputfile.close()
  if outputfile != sys.stdout: outputfile.close()

if __name__ == '__main__':
  main()
