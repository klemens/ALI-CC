# Analyse CommonCrawl

Seminarvortrag im Modul „Anwendungen Linguistische Informatik“

## Ziel

Analyse des [CommonCrawl][CC] Crawl-Prozesses, speziell bezüglich globaler
Abdeckung und Qualität der Textextraktion im Vergleich zu an der Universität
Leipzig benutzten Werkzeugen. Zusätzlich sollen anhand der Metadaten der
gecrawlten Seiten Statistiken über die Verteilung bestimmter Merkmale im Web
erstellt werden.

Details zur Analyse und den Ergebnissen finden sich in der
[Abschlusspräsentation][presentation] und der [Seminararbeit][paper].
Der zugehörige LaTeX-Quelltext befindet sich im Ordner `presentation`.

## Daten

Im Verzeichnis `data` befinden sich die zur Extraktion verwendete
Public-Suffix-Liste von Mozilla und Dateien mit den Pfaden der WAT-Dateien des
CommonCrawl-Crawls vom April 2015, auf dem diese Analyse basiert.

## Werkzeuge

### meta-extractor

Der meta-extractor extrahiert aus einer WAT-Datei (WARC-Format, JSON Payload)
für jede gecrawlte Seite einige Merkmale wie Domain, Webserver, Cookies,
Verwendung populärer Skripte wie jQuery oder das Verhältnis externer zu
interner Links. Eine Liste aller extrahierten Attribute findet sich im
[Wiki][Datenextraktion]. Die extrahierten Daten werden anschließend in einer
csv-Datei zur Weiterverarbeitung gespeichert.

### batch-extract

Dieses Skript läd nacheinander alle WAT-Dateien herunter, extrahiert daraus
unter Verwendung von meta-extractor die csv-Dateien und läd diese anschließend
zur weiteren Analyse auf einen zentralen Server.

### shuffle-lines

shuffle-lines ist ein kleines Hilfsprogramm, um die Zeilen einer Daten zufällig
anzuordnen. Dabei kommt der PRNG Mersenne-Twister 19937 zum Einsatz, wobei der
Seed manuell festgelegt werden kann.

[paper]: ./presentation/paper.pdf
[presentation]: ./presentation/presentation.pdf
[CC]: https://commoncrawl.org/
[Datenextraktion]: https://github.com/klemens/ALI-CC/wiki/Datenextraktion
