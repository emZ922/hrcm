# hrcm

Project that implements Hybrid Referential Compression Method. It is created for Bioinformatics 1 (https://www.fer.unizg.hr/predmet/bio1) class at FER, Zagreb.

****************************************************************************

INSTRUCTIONS

Requirements: git, C++ compiler

1. clone main branch of repository
2. compile files for compression and decompression

g++ -o c.out compressionArguments.cpp
g++ -o d.out decompressionArguments.cpp

3. run executables

./c.out compress -r ref_seq.fa -t t_seq.fa
./d.out decompress -r ref_seq.fa -t t_seq.bin  

4. check output
1 compressed file named t_seq.bin 
2 decompressed file named t_seq.fasta

5. compare input and output
diff t_seq.fa t_seq.fasta

***************************************************************************