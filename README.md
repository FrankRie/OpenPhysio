Voraussetuung für das Build ist eine (OpenSource) Installation von QT6 unter Linux oder QT6 mit Mit MingW  für Windows.

Lade die Quellen komplett von github herunter und configuriere im Qt Creator die beiden Projekte "Opa" und "Openphysio" jeweils dur Laden des entsprechenden CMake.txt aus dem Verzeichnis opa bzw. Openphysio.
In beiden Projekten ist als Ausführverzeichnis ... /run einzustellen.

Erzeuge eine Datenbank (am Besten mit MariaDB) mit dem Namen openphysio und dem Passwort op-pass ( Passwort später anpassen!)
Die Notwendigen Anweisungen zur einrichtung der Datenbank sowie der Generierung der Tabellen sind unter openpysio/database zu finden.

Starte zunächst opa als Admintrator und richte die Grundinformation für die Praxis ein. 

Danach kannst Du openphysio starten und mit der Einrichtung von Patienten beginnen.
