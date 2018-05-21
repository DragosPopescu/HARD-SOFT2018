import time

def citeste_litera_noua(cuvant_de_ghicit = [],cuvant_ghicit = [],litere_utilizate = [],nivel=[]):
    comanda=input()
    #verifica daca a fost data comanda de stop
    if (comanda=="stop"):
        print(" Iesiti din joc? (D=DA/N=NU)")
        optiune=input()
        if (optiune=="D" or optiune=="d"):
            return 3
        else:
            return 0
    #verifica daca s-a introdu o litera care nu a mai fost utilizata
    elif (len(comanda)==1 and comanda.isalpha() and (not litere_utilizate.count(comanda))):
        litere_utilizate.append(comanda)
        litere_utilizate.sort()
        #verifica daca litera apartine cuvantului de ghicit
        if (cuvant_de_ghicit.count(comanda)):
            for i in range (0,len(cuvant_de_ghicit)):
                if (comanda==cuvant_de_ghicit[i]):
                    cuvant_ghicit[i]=comanda
            test=1
            for i in range (0,len(cuvant_de_ghicit)):
                if(not(cuvant_de_ghicit[i]==cuvant_ghicit[i])):
                    test=0
            if (test):
                return 1
            else:
                return 0
        else:
            nivel[0]=nivel[0]+1
            if (nivel[0]>8):
                return 2
            else:
                return 0
    else:
        print(" Comanda/litera neacceptata")
        time.sleep(1)
        return 0


