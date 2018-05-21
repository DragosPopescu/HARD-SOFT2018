import os
import sys
from JOC import JocSpnazuratoare

#Setare dimensiune consola
os.system('mode con: cols=100 lines=42')
joc=JocSpnazuratoare()

#meniul principal
while(1):
    os.system('cls')
    #afiseaza optiunile
    print()
    print("   Apasa tasta:")
    print("     '1' pentru a incepe jocul")
    print("     '2' pentru a iesi din joc")

    optiune=input()
    #optiune pentru joc nou
    if (optiune=="1"):
        joc.start_joc()
    #optune pentru a iesi din joc
    if (optiune=="2"):
        #goleste consola
        os.system('cls')
        print(" Iesiti din joc? (D=DA/N=NU)")
        optiune=input()
        if (optiune=="D" or optiune=="d"): sys.exit()
