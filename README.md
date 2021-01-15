# Fingerprint-o-tron
Document fingerprinter<br>
Winnowing document fingerprinting<br>
A C++ implementation of this algorithm:

http://igm.univ-mlv.fr/~mac/ENS/DOC/sigmod03-1.pdf

To build it requires CMake and GNU C++ (min version 4.8) and ICU (for Debian apt-get install libicu-dev). Once built it produces a utility "compare". You pass it the files (UTF-8 text) to compare against each other. The utility can take either directories or individual files, it will work out what to do. It will then fingerprint each document and compare the results with every other document. As each document is compared it outputs the percentage of fingerprints that are common to both documents. Finally it lists all the documents that are similar to each other (as in have a > 20% fingerprint match).
<br>
Example output (running against directory called "examples" containing 4 text files):<br>
<br>
./compare examples<br>
<br>
Comparing documents with the following parameters:<br>
NGramSize = 10<br>
WinnowSize = 9<br>
Threshold = 20<br>
<br>
Processing files:<br>
examples/random.txt<br>
examples/test1.txt<br>
examples/test1_same.txt<br>
examples/test2.txt<br>
<br>
examples/random.txt - examples/test1.txt %2<br>
examples/random.txt - examples/test1_same.txt %2<br>
examples/random.txt - examples/test2.txt %2<br>
examples/test1.txt - examples/test1_same.txt %100<br>
examples/test1.txt - examples/test2.txt %65<br>
examples/test1_same.txt - examples/test2.txt %65<br>
<br>
Similar documents:<br>
examples/test1.txt examples/test1_same.txt examples/test2.txt<br>
<br>
so test1.txt has 65% of its fingerprints in common with test2.txt. rand.txt only has 2% in common with test1.txt.<br>
<br>
test1.txt / test1_same.txt and test2.txt all have common fingerprints.<br>
