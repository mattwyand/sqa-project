# **CSCI 3060U, Phase 1: Test Plan**

**Team Members:** Sully Butt, Mathew Wyand, Mathew Delaney

**Objective:** This document will explain how the Front- End requirements tests are organized, how they will be run, and how their results will be stored and compared. 

**Folder Tree:** 

Folder PATH listing for volume DATA  
Volume serial number is 525E-D13E  
D:\\SOFTWAREQUALITYASSURANCE\\SQA-PROJECT\\TESTCASES  
\+---Inputs  
|   \+---addcredit  
|   |   \\---input  
|   |           37\_addcredit\_standard\_valid\_input.txt  
|   |           38\_addcredit\_admin\_valid\_target\_input.txt  
|   |           39\_addcredit\_admin\_invalid\_target\_input.txt  
|   |           40\_addcredit\_1000\_boundary\_input.txt  
|   |           41\_addcredit\_over\_1000\_single\_input.txt  
|   |           42\_addcredit\_over\_1000\_cumulative\_input.txt  
|   |             
|   \+---bad\_input  
|   |   \\---input  
|   |           45\_unknown\_transaction\_input.txt  
|   |           46\_sell\_non\_numeric\_price\_input.txt  
|   |           47\_refund\_non\_numeric\_amount\_input.txt  
|   |           48\_addcredit\_non\_numeric\_amount\_input.txt  
|   |             
|   \+---buy  
|   |   \\---input  
|   |           27\_buy\_valid\_game\_input.txt  
|   |           28\_buy\_sell\_standard\_rejected\_input.txt  
|   |           29\_buy\_nonexistent\_game\_input.txt  
|   |           30\_buy\_insufficient\_credit\_input.txt  
|   |           31\_buy\_already\_owned\_game\_input.txt  
|   |             
|   \+---create  
|   |   \\---input  
|   |           08\_create\_valid\_user\_input.txt  
|   |           09\_create\_each\_user\_type\_input.txt  
|   |           10\_create\_non\_admin\_rejected\_input.txt  
|   |           11\_create\_duplicate\_username\_input.txt  
|   |           12\_create\_username\_too\_long\_input.txt  
|   |           13\_create\_username\_length\_boundary\_input.txt  
|   |             
|   \+---delete  
|   |   \\---input  
|   |           14\_delete\_valid\_user\_input.txt  
|   |           15\_delete\_non\_admin\_rejected\_input.txt  
|   |           16\_delete\_nonexistent\_user\_input.txt  
|   |           17\_delete\_current\_user\_rejected\_input.txt  
|   |           18\_deleted\_seller\_inventory\_unavailable\_input.txt  
|   |             
|   \+---list  
|   |   \\---input  
|   |           43\_list\_available\_games\_input.txt  
|   |           44\_list\_before\_login\_rejected\_input.txt  
|   |             
|   \+---login\_logout  
|   |   \\---input  
|   |           01\_valid\_admin\_login\_input.txt  
|   |           02\_valid\_standard\_login\_input.txt  
|   |           03\_invalid\_username\_login\_input.txt  
|   |           04\_transaction\_before\_login\_input.txt  
|   |           05\_second\_login\_rejected\_input.txt  
|   |           06\_logout\_without\_login\_input.txt  
|   |           07\_only\_login\_after\_logout\_input.txt  
|   |             
|   \+---refund  
|   |   \\---input  
|   |           32\_refund\_valid\_input.txt  
|   |           33\_refund\_non\_admin\_rejected\_input.txt  
|   |           34\_refund\_missing\_buyer\_input.txt  
|   |           35\_refund\_missing\_seller\_input.txt  
|   |           36\_refund\_over\_seller\_balance\_requirement\_gap\_input.txt  
|   |             
|   \\---sell  
|       \\---input  
|               19\_sell\_valid\_game\_input.txt  
|               20\_sell\_buy\_standard\_rejected\_input.txt  
|               21\_sell\_price\_max\_boundary\_input.txt  
|               22\_sell\_price\_over\_max\_input.txt  
|               23\_sell\_game\_name\_25\_chars\_input.txt  
|               24\_sell\_game\_name\_too\_long\_input.txt  
|               25\_sell\_duplicate\_game\_name\_input.txt  
|               26\_new\_sale\_not\_usable\_same\_session\_input.txt  
|                 
\\---Outputs  
    \+---addcredit  
    |   \\---expected  
    |           37\_addcredit\_standard\_valid\_expected.txt  
    |           38\_addcredit\_admin\_valid\_target\_expected.txt  
    |           39\_addcredit\_admin\_invalid\_target\_expected.txt  
    |           40\_addcredit\_1000\_boundary\_expected.txt  
    |           41\_addcredit\_over\_1000\_single\_expected.txt  
    |           42\_addcredit\_over\_1000\_cumulative\_expected.txt  
    |             
    \+---bad\_input  
    |   \\---expected  
    |           45\_unknown\_transaction\_expected.txt  
    |           46\_sell\_non\_numeric\_price\_expected.txt  
    |           47\_refund\_non\_numeric\_amount\_expected.txt  
    |           48\_addcredit\_non\_numeric\_amount\_expected.txt  
    |             
    \+---buy  
    |   \\---expected  
    |           27\_buy\_valid\_game\_expected.txt  
    |           28\_buy\_sell\_standard\_rejected\_expected.txt  
    |           29\_buy\_nonexistent\_game\_expected.txt  
    |           30\_buy\_insufficient\_credit\_expected.txt  
    |           31\_buy\_already\_owned\_game\_expected.txt  
    |             
    \+---create  
    |   \\---expected  
    |           08\_create\_valid\_user\_expected.txt  
    |           09\_create\_each\_user\_type\_expected.txt  
    |           10\_create\_non\_admin\_rejected\_expected.txt  
    |           11\_create\_duplicate\_username\_expected.txt  
    |           12\_create\_username\_too\_long\_expected.txt  
    |           13\_create\_username\_length\_boundary\_expected.txt  
    |             
    \+---delete  
    |   \\---expected  
    |           14\_delete\_valid\_user\_expected.txt  
    |           15\_delete\_non\_admin\_rejected\_expected.txt  
    |           16\_delete\_nonexistent\_user\_expected.txt  
    |           17\_delete\_current\_user\_rejected\_expected.txt  
    |           18\_deleted\_seller\_inventory\_unavailable\_expected.txt  
    |             
    \+---list  
    |   \\---expected  
    |           43\_list\_available\_games\_expected.txt  
    |           44\_list\_before\_login\_rejected\_expected.txt  
    |             
    \+---login\_logout  
    |   \\---expected  
    |           01\_valid\_admin\_login\_expected.txt  
    |           02\_valid\_standard\_login\_expected.txt  
    |           03\_invalid\_username\_login\_expected.txt  
    |           04\_transaction\_before\_login\_expected.txt  
    |           05\_second\_login\_rejected\_expected.txt  
    |           06\_logout\_without\_login\_expected.txt  
    |           07\_only\_login\_after\_logout\_expected.txt  
    |             
    \+---refund  
    |   \\---expected  
    |           32\_refund\_valid\_expected.txt  
    |           33\_refund\_non\_admin\_rejected\_expected.txt  
    |           34\_refund\_missing\_buyer\_expected.txt  
    |           35\_refund\_missing\_seller\_expected.txt  
    |           36\_refund\_over\_seller\_balance\_requirement\_gap\_expected.txt  
    |             
    \\---sell  
        \\---expected  
                19\_sell\_valid\_game\_expected.txt  
                20\_sell\_buy\_standard\_rejected\_expected.txt  
                21\_sell\_price\_max\_boundary\_expected.txt  
                22\_sell\_price\_over\_max\_expected.txt  
                23\_sell\_game\_name\_25\_chars\_expected.txt  
                24\_sell\_game\_name\_too\_long\_expected.txt  
                25\_sell\_duplicate\_game\_name\_expected.txt  
                26\_new\_sale\_not\_usable\_same\_session\_expected.txt  
                

               

Explanation: Each transaction type has its own folder(login\_logout, create, delete, sell, buy, refund, addcredit, list, bad\_input).

1. The inputs folders will hold what a user would type. Whereas outputs folder will hold what the program should respond. Additionally, every test has a name and number. Both input and output files will share the same number and name.

   

   1. For example: 32\_refund\_valid\_input.txt and 32\_refund\_valid\_expected.txt  
        
   2. **ALL tests ASSUME a starting user accounts file and available games file, which will be provided with the front end in Phase 2\.**

How the tests will run: A Powershell script “run\_tests.ps1”, will feed each input file to the front end and save what the program prints. This script will be run each time we test the front end, starting one it is built in Phase 2\.

1. Each time the script runs, it creates a new folder in TestResults named with date and time. It saves the actual output there. We compare each actual output file to its expected output file and record **Pass** or **Fail**. Because expected outputs are in English, a team member will read each actual output file and compares it to its expected file by hand. Keeping a folder per run lets us directly compare results between runs.  
2. Script for run\_tests.ps1:  
   

| \# run\_tests.ps1 \# Feeds every test input file to the front end and saves what the program prints.  \$program \= ".\\frontend\\frontend.exe"        \# the program (built in Phase 2\) \$runDir  \= ".\\TestResults\\run\_" \+ (Get-Date \-Format "yyyy-MM-dd\_HHmm") \# Find every input file in every folder under TestCases\\Inputs \$inputs \= Get-ChildItem \-Path ".\\TestCases\\Inputs" \-Recurse \-Filter "\*\_input.txt"  foreach (\$file in \$inputs) {     \$testName \= \$file.Name \-replace "\_input.txt", ""     \$category \= \$file.Directory.Parent.Name     \# make a results folder for this category, e.g. TestResults\\run\_...\\sell     New-Item \-ItemType Directory \-Force \-Path "\$runDir\\\$category" | Out-Null      \# run the program using the input file, and save what it prints     Get-Content \$file.FullName | & \$program \> "\$runDir\\\$category\\\${testName}\_actual.txt" }  |
| :---- |

   

**Disclaimer**: Expected output are written as descriptive english since the program does not exist yet and prompt wording is unspecified. Results will be compared by review and then refined into exact text in later phases. 

