class Parent(object):
	def implicit(self):
	       print "PARENT implicit()"
class Child(Parent):
	def implicit(self):
		print "child implicit()"
		super(Child, self).implicit();
		print "child implicit()"
dad = Parent()
son = Child()

dad.implicit()
son.implicit()
