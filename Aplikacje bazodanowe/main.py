# -*- coding: utf-8 -*-

# polskie znaki

import oracledb
from random import randint
import datetime

# -------------------------------------- FUNCTION SECTION --------------------------------------

def wyswietl(x):
    zapytanie_pola = "SELECT column_name FROM user_tab_columns WHERE table_name = '"+tabele[x-1].rstrip("\n")+"'"
    #print(zapytanie_pola)
    cur = connection.cursor()
    cur.execute(zapytanie_pola)
    nazwy_pol = cur.fetchall()
    cur.close()
    #connection.close()
    print("\nWYBRANO TABELE "+tabele[x-1])
    print()
    #print("Atrybuty tej tabeli:")
    for i in range (0, len(nazwy_pol)):
        nazwa_pola = str(nazwy_pol[i])
        nazwa_pola = nazwa_pola.rstrip("',)") # Usunięcie cudzysłowu i nawiasu z prawej strony
        nazwa_pola = nazwa_pola.lstrip("('") # Usunięcie nawiasu i cudzysłowu z lewej strony
        nazwa_pola = nazwa_pola.ljust(25) # Ustawienie stałej odległości początku tekstu od lewej krawędzi
        print(nazwa_pola, end="")
    pole_id = str(nazwy_pol[0])
    pole_id = pole_id.rstrip("',)")  # Usunięcie cudzysłowu i nawiasu z prawej strony
    pole_id = pole_id.lstrip("('")  # Usunięcie nawiasu i cudzysłowu z lewej strony

    #
    zapytanie_dane = "SELECT * FROM "+tabele[x-1].rstrip("\n")+" ORDER BY "+pole_id+" ASC"
    #print("\n"+zapytanie_dane)
    cur = connection.cursor()
    cur.execute(zapytanie_dane)
    dane_tabeli = cur.fetchall()
    cur.close()

    print()
    for i in range(0, len(dane_tabeli)):
        rekord = dane_tabeli[i]
        for j in range(0, len(rekord)):
            pole = str(rekord[j])
            pole = pole.ljust(25)  # Ustawienie stałej odległości między wartościami pól
            print(pole, end="")
        print()


def dodawanie(nr_tab):
    nr_tab = nr_tab-1

    zapytanie_pola = "SELECT column_name FROM user_tab_columns WHERE table_name = '" + tabele[nr_tab].rstrip("\n") + "'"
    # print(zapytanie_pola)
    cur = connection.cursor()
    cur.execute(zapytanie_pola)
    nazwy_pol = cur.fetchall()
    cur.close()
    for i in range (0, len(nazwy_pol)):
        nazwy_pol[i] = str(nazwy_pol[i])
        nazwy_pol[i] = nazwy_pol[i].rstrip("',)") # Usunięcie cudzysłowu i nawiasu z prawej strony
        nazwy_pol[i] = nazwy_pol[i].lstrip("('") # Usunięcie nawiasu i cudzysłowu z lewej strony
    #
    cur = connection.cursor()
    cur.execute("SELECT MAX("+nazwy_pol[0]+") FROM "+tabele[nr_tab])
    result = cur.fetchone()
    cur.close()
    dane = [len(nazwy_pol)]
    dane[0] = result[0]+1
    #print(dane)

    print("DODAWANIE do tabeli "+tabele[nr_tab])
    for i in range (1, len(nazwy_pol)):
        pom = input("Podaj wartosc dla pola "+nazwy_pol[i]+": ")
        dane.append(pom)

    sql_insert = "INSERT INTO "+tabele[nr_tab]+" VALUES (:1"

    for i in range(2, len(nazwy_pol)+1):
        sql_insert = sql_insert + ", :"+str(i)
    sql_insert = sql_insert + ")"
    #
    cur = connection.cursor()
    cur.execute(sql_insert, dane)
    connection.commit()
    cur.close()

    input("Record zostal pomyslnie dodany")


def usuwanie(nr_tab):
    nr_tab = nr_tab-1

    zapytanie_pola = "SELECT column_name FROM user_tab_columns WHERE table_name = '" + tabele[nr_tab].rstrip("\n") + "'"
    # print(zapytanie_pola)
    cur = connection.cursor()
    cur.execute(zapytanie_pola)
    nazwy_pol = cur.fetchall()
    cur.close()
    for i in range(0, len(nazwy_pol)):
        nazwy_pol[i] = str(nazwy_pol[i])
        nazwy_pol[i] = nazwy_pol[i].rstrip("',)")  # Usunięcie cudzysłowu i nawiasu z prawej strony
        nazwy_pol[i] = nazwy_pol[i].lstrip("('")  # Usunięcie nawiasu i cudzysłowu z lewej strony

    print("USUWANIE z tabeli " + tabele[nr_tab])
    usun_id = input("Podaj ID recordu ktory chcesz usunac: ")
    sql_delete = "DELETE FROM "+ tabele[nr_tab] + " WHERE "+ nazwy_pol[0] + " = "+ usun_id
    #print(sql_delete)
    cur = connection.cursor()
    cur.execute(sql_delete)
    connection.commit()
    cur.close()
    input("Record zostal pomyslnie usuniety")


def edytowanie(nr_tab):
    nr_tab = nr_tab - 1

    zapytanie_pola = "SELECT column_name FROM user_tab_columns WHERE table_name = '" + tabele[nr_tab].rstrip("\n") + "'"
    # print(zapytanie_pola)
    cur = connection.cursor()
    cur.execute(zapytanie_pola)
    nazwy_pol = cur.fetchall()
    cur.close()
    for i in range(0, len(nazwy_pol)):
        nazwy_pol[i] = str(nazwy_pol[i])
        nazwy_pol[i] = nazwy_pol[i].rstrip("',)")  # Usunięcie cudzysłowu i nawiasu z prawej strony
        nazwy_pol[i] = nazwy_pol[i].lstrip("('")  # Usunięcie nawiasu i cudzysłowu z lewej strony

    print("EDYTOWANIE tabeli " + tabele[nr_tab])

    edit_id = input("Wybierz ID recordu ktory chcesz edytowac: ")
    edit_pole = int(input("Wybierz pole ktore chcesz edytowac(od 1 do "+str(len(nazwy_pol)-1)+"): "))
    edit_wartosc = input("Podaj nowa wartosc dla pola " + nazwy_pol[(edit_pole)] + ": ")

    sql_edit = ""
    if edit_wartosc.isnumeric():
        sql_edit = "UPDATE " + tabele[nr_tab] + " SET " + nazwy_pol[edit_pole] + " = " + edit_wartosc + " WHERE " + nazwy_pol[0] + " = " + edit_id
    else:
        sql_edit = "UPDATE " + tabele[nr_tab] + " SET " + nazwy_pol[edit_pole] + " = '" + edit_wartosc + "' WHERE " + nazwy_pol[0] + " = " + edit_id

    cur = connection.cursor()
    cur.execute(sql_edit)
    connection.commit()
    cur.close()

    #print(sql_edit)
    #input("wait")

    input("Record zostal pomyslnie zaktualizowany")



# -------------------------------------- END OF FUNCTION SECTION --------------------------------------

print("Start")

# -------------------------------------- Polaczenie z baza --------------------------------------
un = 's101374' #username system
pw = 'piros2000' #password localhost
cs = '217.173.198.135:1521/tpdb' # --- CONNECTION STRING: hostname:port/(servicename or SID) localhost:1522/xe
connection = oracledb.connect(user=un, password=pw, dsn=cs)
print("Połączono z bazą danych")


# -------------------------------------- MAIN --------------------------------------

cur = connection.cursor()
zap_tabele = "SELECT table_name FROM user_tables"
cur.execute(zap_tabele)
tabele = cur.fetchall()
cur.close()

for i in range(0, len(tabele)):
    tabele[i] = str(tabele[i])
    tabele[i] = tabele[i].rstrip("',)")  # Usunięcie cudzysłowu i nawiasu z prawej strony
    tabele[i] = tabele[i].lstrip("('")  # Usunięcie nawiasu i cudzysłowu z lewej strony
    #print(tabele[i])

while(True):
    print("\nWybierz tabele sposrod dostepnych lub zakoncz:\n")
    print("0 - EXIT\n")
    for i in range(0, len(tabele)):
        print(str(i + 1) + " - " + tabele[i])
    ktora_tabela = int(input("\nWybierz tabele: "))

    if ktora_tabela == 0:
        break;
    elif (ktora_tabela > 0 and ktora_tabela < len(tabele)+1):
        wyswietl(ktora_tabela)

        print("\nWybierz operacje na tabeli: \n0 - EXIT\n1 - Dodaj rekord\n2 - Usun rekord\n3 - Edytuj")
        operacja = input("Wybierz operacje: ")
        if operacja == "0":
            break;
        elif operacja == "1": #opcja dodawania
            dodawanie(ktora_tabela)
        elif operacja == "2": #opcja usuwania
            usuwanie(ktora_tabela)
        elif operacja == "3": #opcja edycji
            edytowanie(ktora_tabela)
    else:
        print("\n--- Invalid input! Try again ---")



connection.close()