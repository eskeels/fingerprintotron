# Fingerprint-o-tron
Document fingerprinter<br>
Winnowing document fingerprinting<br>
A C++ implementation of this algorithm:

http://igm.univ-mlv.fr/~mac/ENS/DOC/sigmod03-1.pdf

Document fingerprinting is used to find duplicate text data within other documents. The duplicated text being searched for can be a fragment or the whole document. The algorithm allows for matching text that has been modified. The algorithm is detailed in the PDF but here is a quick description. My implementation also diverges at the winnowing step (detailed below).<br>Given some input text:
<pre>
A do run run run, a do run run
</pre>
Non alphanumeric characters are removed:
```
adorunrunrunadorunrun
```
Ngrams extracted (using a rolling window of 5 characters)
```
adoru dorun orunr runru unrun nrunr runru
unrun nruna runad unado nador adoru dorun
orunr runru unrun
```
Each Ngram is hashed (ie adoru -> 77, dorun -> 74 etc...):
```
77 74 42 17 98 50 17 98 8 88 67 39 77 74 42
17 98
```

The smallest hash per window is selected (this is winnowing):
<pre>
[77 74 42 <b>17</b> 98] [50 17 98 <b>8</b> 88] [67 <b>39</b> 77 74 42]
[<b>17</b> 98]
</pre>
I used a fixed window, the winnowing method actualy uses a rolling window. I found this method to be adequate. It reduced the number of hashes and simplified the code.

Hash is recorded with 0-base position of the hash window:
```
[0,17][1,8][2,39][3,17]
```

# Building
To build it requires CMake and GNU C++ (min version 4.8) and ICU (for Debian apt-get install libicu-dev for Centos or Red Hat yum install libicu-devel). The build steps are simply:<br>
cmake .<br>
make<br>
make test<br>
<br>
Once built it produces a library and a utility "compare". 
# Running
Run "compare" passing the files (UTF-8 text) to compare against each other. The utility can take either directories or individual files, it will work out what to do. It will then fingerprint each document and compare the results with every other document. As each document is compared it outputs the percentage of fingerprints that are common to both documents. Finally it lists all the documents that are similar to each other (as in have a > 20% fingerprint match).
<br>
Example output (running against directory called "examples" containing 4 text files):<br>
<br>
```
./compare examples

Comparing documents with the following parameters:
NGramSize = 10
WinnowSize = 9
Threshold = 20

Processing files:
examples/random.txt
examples/test1.txt
examples/test1_same.txt
examples/test2.txt

examples/random.txt - examples/test1.txt %2
examples/random.txt - examples/test1_same.txt %2
examples/random.txt - examples/test2.txt %2
examples/test1.txt - examples/test1_same.txt %100
examples/test1.txt - examples/test2.txt %65
examples/test1_same.txt - examples/test2.txt %65

Similar documents:
examples/test1.txt examples/test1_same.txt examples/test2.txt
```
so test1.txt has 65% of its fingerprints in common with test2.txt. rand.txt only has 2% in common with test1.txt.<br>
<br>
test1.txt / test1_same.txt and test2.txt all have common fingerprints.<br>
<br>
# Using the library
To use the library from within your own code the unit test "test_system.cpp" is a good starting point, then take a look at main.cpp.<br>
main.cpp loads the text files for comparision, generates the fingerprints and then performs analysis to find all documents deemed similar.
# Fingerprint generation
![Finger print generation sequence diagram](uml/fingerprint_generation_seq.png "Generating fingerprints")
# Compressed mode
As well as normal mode there is also a compressed mode. This uses a fixed number of fingerprints per document. It stores a fixed number of odd and even hashes for each document. The smallest of each type are stored. The sample application uses 200 fingerprints (100 odd / even). When in this mode you should lower the threshold as it is fall less precise.
# Hash size
Hash size is a compile time option. See hasher.h. You can use either HASH32 or HASH64 to select either a 32bit or 64bit hash. 
