import os
import time
from afisare_nivel import afisare_nivel

def afisare_final(final,cuvant_de_ghicit=[]):
    os.system('cls')
    if (final==1):
        afisare_nivel(11)
        print()
        print(" Cuvantul este: ",end="")
        for i in range(0,len(cuvant_de_ghicit)):
            print(cuvant_de_ghicit[i],end="")
        print()
        print()
        time.sleep(2)
    if (final==2):
        print()
        afisare_nivel(9)
        print()
        print(" Cuvantul este: ",end="")
        for i in range(0,len(cuvant_de_ghicit)):
            print(cuvant_de_ghicit[i],end="")
        print()
        time.sleep(1.5)
        os.system('cls')
        afisare_nivel(10)
        time.sleep(2)
