x = 1
y = 0
fin = open("Input.txt","w")
fout = open("Output.txt","w")
while y<1000000:
	s = y*3173+x
	x = x+1
	if x== 3173 :
		x=0
	y = y+1
	fin.write(str(s))
	fin.write(' ')
	fout.write(str(x))
	fout.write(' ')
	
	
