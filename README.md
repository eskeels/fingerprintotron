# Fingerprint-o-tron
Document fingerprinter<br>
Winnowing document fingerprinting<br>
A C++ implementation of this algorithm:

http://igm.univ-mlv.fr/~mac/ENS/DOC/sigmod03-1.pdf

To build it requires CMake and GNU C++ (min version 4.8). Once built it produces a utility "compare". You pass it the files (UTF-8 text) to compare against each other. The utility can take either directories or individual files, it will work out what to do. It will then fingerprint each document and compare the results with every other document. As each document is compared it outputs the percentage of fingerprints that are common to both documents. Finally it lists all the documents that are similar to each other (as in have a > 20% fingerprint match).
<br>
Example output (running against directory called "dir" containing 4 text files):<br>
<br>
Processing files:<br>
dir/test1.txt<br>
dir/test2.txt<br>
dir/test1_same.txt<br>
dir/rand.txt<br>
<br>
dir/test1.txt - dir/test2.txt %24<br>
dir/test1.txt - dir/test1_same.txt %100<br>
dir/test1.txt - dir/rand.txt %1<br>
dir/test2.txt - dir/test1_same.txt %22<br>
dir/test2.txt - dir/rand.txt %5<br>
dir/test1_same.txt - dir/rand.txt %1<br>
Similar documents<br>
dir/test1.txt dir/test1_same.txt dir/test2.txt<br>
<br>
so test1.txt has 24% of its fingerprints in common with test2.txt. rand.txt only has 1% in common with test1.txt.<br>
<br>
test1.txt / test1_same.txt and test2.txt all have common fingerprints.<br>
