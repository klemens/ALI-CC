# Analyse CommonCrawl

Seminarvortrag im Modul „Anwendungen Linguistische Informatik“

## Ziel

Analyse des [CommonCrawl][CC] Crawl-Prozesses, speziell bezüglich globaler
Abdeckung und Qualität der Textextraktion im Vergleich zu an der Universität
Leipzig benutzten Werkzeugen.

Zusätzlich sollen anhand der Metadaten der gecrawlten Seiten Statistiken über
die Verteilung bestimmter Merkmale im Web erstellt werden.

[CC]: https://commoncrawl.org/

## Werkzeuge

### meta-extractor

Der meta-extractor extrahiert aus einer WAT-Datei (WARC-Format, JSON Payload)
für jede gecrawlte Seite einige Merkmale wie Domain, Webserver, Cookies,
Verwendung populärer Skripte wie jQuery oder das Verhältnis externer zu
interner Links. Eine vorläufige Liste möglicher Attribute findet sich im
[Wiki][Datenextraktion]. Die extrahierten Daten werden anschließend in einer
csv-Datei zur Weiterverarbeitung gespeichert.

### batch-extract

Dieses Skript läd nacheinander alle WAT-Dateien herunter, extrahiert daraus
unter Verwendung von meta-extractor die csv-Dateien und läd diese anschließend
zur weiteren Analyse auf einen zentralen Server.

[Datenextraktion]: https://github.com/klemens/ALI-CC/wiki/Datenextraktion
