**Members: Sully Butt, Mathew Delaney, Mathew Wyand**

**Systems**

- Login  
- Searching store games  
- Searching library games  
- Sorting  
- Buying games  
- Friends list  
- User profile  
- Community Workshop  
- List games in library  
- Download Games

- – UNUSED –  
- Discussion board  
- Guides board  
- Achievements

**Adding games to Steam as a developer**

- System requirements (bare minimum and recommended)  
- Uploading screenshots/video  
- 

**Login**:  
List of test cases (and what they are intended to test):  
Prof Update:

Add a new transaction LIST that outputs to the terminal all items currently up for sale and the relevant information

| Behaviour | Test case | Input | Expected Output |
| :---- | :---- | :---- | :---- |
| Create | Username exactly 15 characters | login admin create abcdefghijklmno FS logout | Create accepted at the 15-character boundary. |
| Delete | Delete valid user | login admin delete fulluser logout | User deleted; 02 delete record written. |
| Delete | Delete as non-admin | login fulluser delete buyer logout | Delete rejected because transaction is privileged. |
| Delete | Delete non-existing user | login admin delete ghostuser logout | Delete rejected because user does not exist. |
| Delete | Delete current user | login admin delete admin logout | Delete rejected because current user cannot delete themself. |
| Delete | Deleted user's inventory | login admin delete seller buy Portal 2 seller logout | Buy rejected because deleted user's inventory can no longer be used. |
| Sell | Valid sale | login seller sell New Game 15.00 logout | Sale accepted; 03 sell record written. |
| Sell | Buy-standard attempts sell | login buyer sell New Game 15.00 logout | Sell rejected for BS account. |
| Sell | Price exactly 999.99 | login seller sell Max Price Game 999.99 logout |  |
| Sell | Game name over 25 characters | login seller sell ABCDEFGHIJKLMNOPQRSTUVWXYZ 25.00 logout | Sale rejected because game name exceeds 25 characters. |
| Sell | Duplicate game name | login seller sell Portal 2 30.00 logout | Sale rejected because game names must be unique. |
| Sell | New sale reused immediately | login seller sell Fresh Game 10.00 buy Fresh Game seller logout | Further transaction on newly listed game is rejected until next session. |
| Buy | Valid purchase | login buyer buy Portal 2 seller logout | Purchase accepted; balances updated; game added to buyer collection; 04 record written. |
| Buy | Sell-standard attempts buy | login seller buy Portal 2 seller logout | Buy rejected for SS account. |
| Buy | Non-existing game | login buyer buy No Such Game seller logout | Buy rejected because game does not exist. |
| Buy | Insufficient credit | login lowcredit buy Expensive Game seller logout | Buy rejected because buyer lacks sufficient credit. |
| Buy | Game already owned | login buyer buy Owned Game seller logout | Buy rejected because buyer already owns game. |
| Refund | Valid refund | login admin refund buyer seller 5.00 logout | Refund accepted; 5.00 transferred; 05 record written. |
| Refund | Refund as non-admin | login fulluser refund buyer seller 5.00 logout | Refund rejected because transaction is privileged. |
| Refund | Buyer does not exist | login admin refund ghostbuyer seller 5.00 logout | Refund rejected because buyer is not a current user. |
| Refund | Seller does not exist | login admin refund buyer ghostseller 5.00 logout | Refund rejected because seller is not a current user. |
| Refund | Refund exceeds seller balance | login admin refund buyer seller 999999.00 logout | Requirements problem: expected behaviour must be clarified with TA/client; program must not crash. |
| Add Credit | Standard user adds credit | login buyer addcredit 100.00 logout | Credit added to logged-in user's account; 06 record written. |
| Add Credit | Admin adds credit to user | login admin addcredit 100.00 buyer Logout  | Credit added to existing target user; 06 record written. |
| Add Credit | Admin targets invalid user | login admin addcredit 100.00 ghostuser logout | Add credit rejected because target user does not exist. |
| Add Credit | Add exactly 1000.00 | login buyer addcredit 1000.00 logout | Accepted because session total equals maximum. |
| Add Credit | Add more than 1000.00 | login buyer addcredit 1000.01 logout | Rejected because amount exceeds per-session maximum. |
| Add Credit | Cumulative session limit | login buyer addcredit 600.00 addcredit 400.00 addcredit 0.01 logout | First two accepted; final 0.01 rejected because session total would exceed 1000.00. |
| LIST | List available games | login buyer list logout | All currently available games and relevant sale information are printed. |
| LIST | List before login | list | LIST rejected because no user is logged in. |
| Bad Input | Unknown transaction | login buyer dance logout | Unknown command handled politely; program does not crash. |
| Bad Input | Non-numeric sell price | login seller sell Bad Price Game abc logout | Invalid price rejected or re-prompted; program does not crash. |
| Bad Input | Non-numeric refund amount | login admin refund buyer seller five logout | Invalid refund amount rejected or re-prompted; no invalid record written. |
| Bad Input | Non-numeric add-credit amount | login buyer addcredit money logout | Invalid credit amount rejected or re-prompted; no invalid record written. |

