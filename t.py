import sys

f = open("traceprogs/tr-simpleloop.ref", "r")
line = f.readline()
I = 0
e =0

data = {}
ins = {}

for line in f:

	t = line.split(',')

	if (t[1] == 'I\n'):
		if (t[0] not in ins.keys()):
			ins[t[0]] = 1
		else:
			ins[t[0]] = ins[t[0]] + 1
	else:
		if (t[0] not in data.keys()):
			data[t[0]] = 1
		else:
			data[t[0]] = data[t[0]] + 1
		
print (ins['0x32b6a0ef7c'])
print (ins['0x400520'])
print(len(ins))
print(len(data))
