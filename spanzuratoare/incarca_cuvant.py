import random

# citire linie cu linie din fisier
# construim lista de cuvinte
def citire_fisier():
    lista_cuvinte = []

    with open('lista_cuvinte.txt', 'r') as f:
        for line in f:
           lista_cuvinte.append(line)
    f.close()
    return lista_cuvinte

# aceasta medota primeste ca parametru o lista si returneaza un element random din ea
def cuvant_random(cuvinte):
    return random.choice(cuvinte)


# aceasta metoda primeste un cuvant random si returneaza o lista cu caracterele cuvantului
def incarca_cuvant():
    lista_litere = []
    cuvant = cuvant_random(citire_fisier())
    for i in range(len(cuvant) - 1):
        lista_litere.append(cuvant[i])
    return lista_litere








