if __name__ == '__main__':
	s = ''
	for t in argv:
		if t != argv[0]:
			s += ' '
		s += t
	with open('C:\\Users\\AntiLeaf\\Desktop\\bot\\AntiLeaf-Bot\\src\\temp.txt', 'w') as f:
		f.write(s)