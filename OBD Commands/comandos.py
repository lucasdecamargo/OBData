import obd
import csv

connection = obd.OBD()
print connection.protocol_name()

A = connection.query(obd.commands.PIDS_A)
B = connection.query(obd.commands.PIDS_B)
C = connection.query(obd.commands.PIDS_C)

y = [1 if digit=='1' else 0 for digit in bin(int(str(A),2))[2:]] + [1 if digit=='1' else 0 for digit in bin(int(str(B),2))[2:]] + [1 if digit=='1' else 0 for digit in bin(int(str(C),2))[2:]]

with open('comandos.csv') as csv_file:
	csv_reader = csv.reader(csv_file, delimiter=',')
	i = 0
	for row in csv_reader:
		if(y[i] == 1):
			print row
		i = i +1
