import os
from afisare_nivel import afisare_nivel
from incarca_cuvant import incarca_cuvant
from initializare_cuvant_ghicit import initializare_cuvant_ghicit
from initializare_litere_utilizate import initializare_litere_utilizate
from afisare_litere_utilizate import afisare_litere_utilizate
from afisare_cuvant_ghicit import afisare_cuvant_ghicit
from afisare_instructiuni import afisare_instructiuni
from citeste_litera_noua import citeste_litera_noua
from afisare_final import afisare_final

class JocSpnazuratoare:
    cuvant_de_ghicit = []  #cuvantul care trebuie ghicit
    cuvant_ghicit = []  #partea cuvantului care a fost ghicit
    litere_utilizate = []  #evidenta cu literele utilizate
    nivel =[0]  #evidenta cu numarul de greseli
    final=-1  #indica daca jocul a ajuns la final
              # valoarea -1: joc neinceput
              # valoarea  0: joc neterminat
              # valoarea  1: joc castigat
              # valoarea  2: joc pierdut
              # valoarea  3: joc abandonat

    def afisare_nivel(self):
        afisare_nivel(self.nivel[0])

    def incarca_cuvant(self):
        self.cuvant_de_ghicit.clear()
        self.cuvant_de_ghicit=incarca_cuvant()

    def initializare_cuvant_ghicit(self):
        initializare_cuvant_ghicit(self.cuvant_de_ghicit,self.cuvant_ghicit)

    def initializare_litere_utilizate(self):
        initializare_litere_utilizate(self.cuvant_de_ghicit,self.litere_utilizate)

    def afisare_litere_utilizate(self):
        afisare_litere_utilizate(self.litere_utilizate)

    def afisare_cuvant_ghicit(self):
        afisare_cuvant_ghicit(self.cuvant_ghicit)

    def afisare_instructiuni(self):
        afisare_instructiuni()

    def citeste_litera_noua(self):
        self.final=citeste_litera_noua(self.cuvant_de_ghicit,self.cuvant_ghicit,self.litere_utilizate,self.nivel)

    def afisare_final(self):
        afisare_final(self.final,self.cuvant_de_ghicit)

    def start_joc(self):
        self.nivel[0]=0  #initializeaza nivelul cu 0
        self.final=0
        self.litere_utilizate.clear()
        self.cuvant_de_ghicit.clear()
        self.cuvant_ghicit.clear()
        self.incarca_cuvant()
        self.initializare_cuvant_ghicit()
        self.initializare_litere_utilizate()
        print("aici")
        while(not self.final):
            os.system('cls')
            print()
            self.afisare_nivel()
            print()
            self.afisare_cuvant_ghicit()
            self.afisare_litere_utilizate()
            self.afisare_instructiuni()
            self.citeste_litera_noua()
        self.afisare_final()

