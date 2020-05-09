s = input()

try:
	s = str(eval(s))
except:
	s = '发生错误'

print(s)