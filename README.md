# scrabbled
Scrabble cheat helper utility

List possible word combinations (with associated score) from list of available letters.
 (If list of available letters is given as '-' in command, then actual letter list will subsequently be read from stdin. Use . for unplayed blank.)
In output, scoring letters are shown as lowercase. Non-scoring (blank replacements) are shown as upper case.

&lt;dictionaryFile&gt; is a locally accessible filespec for a list of known words (one word per line)

usage:
        scrabbled {targetLetters|-} &lt;dictionaryFile&gt;

 e.g,,
 
        ./scrabbled bro ~/mydictionary.txt
        2  or
        5  orb
        5  rob

        ./scrabbled br. ~/mydictionary.txt
        1  Dr
        1  Mr
        1  Or
        4  Orb
        4  bAr
        3  bE
        4  bUr
        3  bY
        4  brA
        1  rE
        4  rIb
        4  rOb
        4  rUb

        (- four highest scoring alternatives -)
        ./scrabbled poraens. ~/mydictionary.txt|sort -r -n |head -4|perl -pe "s/\w+\s+(.*)$/\1/"
        personaL
        persona
        pErsonae
        operanDs

