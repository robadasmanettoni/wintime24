# WinTime24

**WinTime24** è un orologio analogico disegnato in tempo reale con **Win32 API**, scritto interamente in C++.  
Non utilizza librerie esterne, è leggero, preciso e compatibile con versioni di Windows a partire da XP.

---

## 🕒 Caratteristiche

- Visualizzazione analogica 12 ore con lancette animate
- Disegno interamente gestito con GDI (`Ellipse`, `LineTo`, `DrawText`)
- Timer con aggiornamento ogni secondo
- Stile semplice, codice leggibile
- Nessuna libreria esterna o framework richiesto

---

## 🛠️ Compilazione

### Con Visual Studio
1. Crea un nuovo progetto *Windows Desktop Application* (Win32 API)
2. Sostituisci il contenuto con `main.cpp`
3. Compila e avvia

### Con Dev-C++
- Nuovo progetto “Windows GUI”
- Inserisci `main.cpp`
- Compila ed esegui

---

## 📄 Licenza

Distribuito con licenza **MIT**.  
Sentiti libero di usare, modificare e ridistribuire il codice.
