#Primeste la intrare doua liste prima ii cuvintul de ghicit iar a doua ii cuvantul ghicit.
# Functia pune prima litera si ultima si alte litere daca mai sunt la mijloc la lista a doua.
def initializare_cuvant_ghicit(lista_litere = [], lista_temporara = [] ):
    lista_temporara.clear()
    for i in range(len(lista_litere)):
        if ( lista_litere[i] == lista_litere[0] or lista_litere[i] == lista_litere[len(lista_litere) - 1]):
            lista_temporara.append(lista_litere[i])
        else:
            lista_temporara.append("_")


