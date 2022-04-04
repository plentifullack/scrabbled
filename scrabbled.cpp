/**
 scrabbled.cpp
 Cheat helper for scrabble game..

 see https://github.com/plentifullack/scrabbled
 (steve hardy <plentifullackofwit@hotmail.com>)
 */

#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <fstream>
#include <set>
#include <regex>
#include <algorithm>

using namespace std;

static const char BLANK = '.';

typedef multimap<string, string> Dictionary;
typedef multimap<string, string>::iterator DictionaryIter;

/// <summary>
/// List of known words, organized for efficient lookup
/// </summary>
Dictionary words;
/// <summary>
/// set of all found word matches
/// </summary>
set<string> matches;

/// <summary>
/// get scrabble score associated with all lowercase letters in given target.
/// </summary>
/// <param name="target">the (unordered) collection of letters to score. Any non-lowercase
/// letters count as zero </param>
/// <returns>calculated score</returns>
int getScore(string target)
{
	int score = 0;
	string mytarget = target;

	for (int i = 0; i < mytarget.length(); i++) {
		string c = mytarget.substr(i, 1);
		if (-1 != c.find_first_of("eaionrtlsu"))
			score += 1;
		else if (-1 != c.find_first_of("dg"))
			score += 2;
		else if (-1 != c.find_first_of("bcmp"))
			score += 3;
		else if (-1 != c.find_first_of("fhvwy"))
			score += 4;
		else if (-1 != c.find_first_of("k"))
			score += 5;
		else if (-1 != c.find_first_of("jx"))
			score += 8;
		else if (-1 != c.find_first_of("qz"))
			score += 10;
	}
	return score;
}

 
/// <summary>
/// normalize any arbitrary ordering of lowercase letters to key that can be used for 
/// dictionary lookup of all similar words
/// </summary>
/// <param name="s">collection of letters from which to produce a lookup key</param>
/// <returns>normalized dictionary key associated with collection of letters in s"></returns>
string getkey(string s)
{
	sort(s.begin(), s.end());
	return s;
}

/// <summary>
/// read in list of all known words (from named text file) into local dictionary store.
/// </summary>
/// <param name="fspec">name (fspec) of the text file containing the list of known words. (Words should be lowercase, with
/// one word per line </param>
/// <returns>true if local dictionary was successfully populated</returns>
bool getDictionary(string fname)
{
	string line;
	ifstream myfile(fname);
	if (myfile.is_open()) {
		while (getline(myfile, line)) {
			words.insert(pair<string, string>(getkey(line), line));
		}
		myfile.close();
		return true;
	}
	return false;
}

int next_combination(vector <int>& c, int k, int n)
{
	int i, j;

	i = k - 1;                            // find next element to increment
	while (c[i] == (n - k + i)) {
		--i;
		if (i < 0) {                     // if done, ...
			for (i = 0; i < k; i++)
				c[i] = i;
			return(0);                  // ... return with initial combination
		}
	}

	c[i] += 1;                          // increment element

	for (j = i + 1; j < k; j++)            // create increasing string
		c[j] = c[i] + j - i;

	return(1);                          // return with new combination
}

/// <summary>
/// get collection of all possible lookup keys associated with target
/// </summary>
/// <param name="target">arbitrary collection of letters, possibly including a BLANK (.)</param>
/// <returns>associated collection of possible lookup keys</returns>
set<string> getCombinations(string target) // undefined behavior for non-ascii input
{
	set<string> combinations;
	int n = (int)target.length();       // n things k at a time
	int i, k;
	int dotCt = (int)std::count(target.begin(), target.end(), BLANK);
	vector <int> c(n);                  // combination array
	std::string lcstr;
	// Allocate the destination space
	lcstr.resize(target.size());

	// fixup any (sloppy)uppercase input
	// Convert the source string to lower case
	// storing the result in destination string
	// (note: Assuming ASCII, !!!)
	std::transform(target.begin(),
		target.end(),
		lcstr.begin(),
		::tolower);
	lcstr.erase(std::remove(lcstr.begin(), lcstr.end(), BLANK), lcstr.end());
	target = lcstr;
	sort(target.begin(), target.end());
	for (k = 2; k <= n; k++) {            // n things k at a time (http://stackoverflow.com/a/27461057)
		for (i = 0; i < k; i++)         // create initial combination
			c[i] = i;
		do {                             // do combination
			string comb = "";
			for (i = 0; i < k; i++) {
				comb += target[c[i]];
			}
			if (combinations.end() == combinations.find(comb.c_str())) { 
				combinations.insert(comb.c_str());
			}
		} while (next_combination(c, k, n));
	}
	if (dotCt > 0) {
		set<string> dots;
		for (string s : combinations) {
			string sdot("." + s);
			for (int i = 0; i < sdot.length(); ++i) {
				if (i > 0)
					swap(sdot[i - 1], sdot[i]);
				dots.insert(sdot);
			}
		}
		// (note: only supporting a single blank)
		//        if (2==dotCt){
		//            set<string> dot2s;
		//            for (string s:dots){
		//                string sdot("."+s);
		//                for (int i=0;i<sdot.length();++i){
		//                    if (i>0)
		//                        swap(sdot[i-1],sdot[i]);
		//                    dot2s.insert(sdot);
		//                }
		//            }
		//            for (string s:dot2s)
		//                dots.insert(s);
		//        }
		for (string s : dots)
			combinations.insert(s);
	}
	return combinations;
}

/// <summary>
/// If key contains any uppercase (as a result of matched BLANK), transform corresponding 
/// letter in s accordingly, before inserting s into matches collection.
/// </summary>
/// <param name="s">lowercase word matched from dictionary</param>
/// <param name="scoreKey">dictionary lookup key, which may contain an uppercase letter due a BLANK match</param>
void handleMatch(string s, string scoreKey)
{
	// if scoreKey contains any uppercase, transform s accordingly
	if (0 != s.compare(scoreKey)) {
		for (auto it = scoreKey.begin(); it != scoreKey.end(); ++it) {
			if (isupper(*it)) {
				for (int i = 0; i < s.length(); ++i) {
					if (*it == toupper(s[i])) {
						s[i] = toupper(s[i]);
						break;
					}
				}
			}
		}
	}
	matches.insert(s);
}

/**
insert s into matches collection
**/
void handleMatch(string s)
{
	handleMatch(s, s);
}

string entireLineRegex(string s)
{
	return string("^") + s + string("$");
}


int nextRot(string& s, int& curOffset)
{
	if (curOffset < s.length()) {
		swap(s[0], s[curOffset]);
		curOffset++;
	}
	while (curOffset << s.length() && s[0] == s[curOffset]) {
		curOffset++;
	}
	if (curOffset < s.length()) {
		swap(s[0], s[curOffset]);
	}
	return curOffset;
}


/**
 build list of words that can be built from collection letters inicated by target
 target is letters we're holding, possibly including a single BLANK (.)
 Precondition:
 -caller should have already successfully invoked getDictionary().
 -caller should have already forced lowercase and sorted target.
 On return:
 -matches collection will be populated with list of word match- candidates.
*/
void getScrabbleWords(string target)
{
	string lcTarget(target);

	set<string> combinations = getCombinations(lcTarget);
	pair<   DictionaryIter,
		DictionaryIter> ii;
	DictionaryIter it; //Iterator to be used along with ii

	for (std::set<string>::const_iterator aCombination = combinations.begin(); aCombination != combinations.end(); ++aCombination) {
		long blankAt = aCombination->find(BLANK);
		if (string::npos != blankAt) {
			// we need to try matches for . in each position
			string regexStr(*aCombination);
			string lastKey("");
			set<string> matchedKeys;
			regex thisRegex(regexStr);
			// get set of keys matching this regex
			for (DictionaryIter it2 = words.begin(); it2 != words.end(); ++it2) {
				if (0 != lastKey.compare(it2->first))
				{
					if (regex_match(it2->first, thisRegex)) {
						string key = it2->first;
						key[blankAt] = toupper(key[blankAt]);
						matchedKeys.insert(key);
					}
					lastKey = it2->first;
				}
			}
			typedef set<string>::iterator KeyIterator;
			for (KeyIterator key = matchedKeys.begin(); key != matchedKeys.end(); ++key) {
				// key is potentially mixed case, indicating BLANK or otherwise unscored positions
				string lckey = *key;
				std::transform(lckey.begin(), lckey.end(), lckey.begin(), ::tolower);
				ii = words.equal_range(lckey); //We get the first and last entry in ii;

				for (it = ii.first; it != ii.second; ++it) {
					handleMatch(it->second, *key);
				}
			}
		}
		else {
			ii = words.equal_range(*aCombination); // all words for this combo/key

			for (it = ii.first; it != ii.second; ++it) {
				handleMatch(it->second);
			}
		}
	}
}

/// <summary>
/// print (on stdout) list of matched words (from named dictionary file) that can be built from 
/// arrangements of target letters, along with associated scrabble score of each word match.
/// 
/// </summary>
/// <param name="target">The unordered collection of letters available to use for word building.
/// Any uppercase letters in input are first silently transformed to lowercase. 
/// A dot (.) can be used to indicate a single blank (i.e., wildcard) tile. A matched letter
/// associated with use of a blank tile shows as uppercase in the output list and counts for 
/// zero in the resulting word score.
/// </param>
/// <param name="dictionaryName">The name of text file holding list of known words (one word per line)</param>
/// <returns>
/// 0 if list was successfully printed, or nonzero
///  on error (no list could be printed)</returns>
int showScrabbleWords(string target, string dictionaryName)
{
	if (getDictionary(dictionaryName)) {
		std::string lcTarget;

		std::string current_key = "";
		//        for (auto itr = words.begin(); itr != words.end(); ++itr)
		//            if(current_key == itr->first)
		//                continue;
		//            else{
		//                current_key = itr->first;
		//                const auto result = words.equal_range(itr->first);
		//                cout << itr->first << ": ";
		//                for(auto it = result.first; it != result.second; ++it)
		//                    cout << it->second << " ";
		//                cout << endl;
		//            }

		// Allocate the destination space
		lcTarget.resize(target.size());
		std::transform(target.begin(),
			target.end(),
			lcTarget.begin(),
			::tolower);
		sort(lcTarget.begin(), lcTarget.end());

		// disallow (silently ignore) more than one blank 
		while (lcTarget.length() > 1 && lcTarget[1] == BLANK) {
			lcTarget = lcTarget.substr(1);
		}

		getScrabbleWords(lcTarget);

		for (set<string>::iterator i = matches.begin(); i != matches.end(); ++i) {
			cout << "" << getScore(*i) << "  " << *i << endl;
		}
		return 0;
	}
	cerr << "ERROR. Unable to read dictionary: " + dictionaryName << endl;
	return 1;
}

/// <summary>
/// print help message (with usage examples) to stdout
/// </summary>
void showHelp() {
	cout << endl << "scrabble cheat helper. " << endl
		<< "List possible word combinations (with associated score) from list of available letters." << endl << " (If list of available letters is given as '-' in command, then actual letter list will subsequently be read from stdin. Use . for unplayed blank.)" << endl
		<< "In output, scoring letters are shown as lowercase. Non-scoring (blank replacements) are shown as upper case." << endl << endl
		<< "<dictionaryFile> is a locally accessible filespec for a list of known words (one word per line)" << endl << endl
		<< "usage:" << endl << "\tscrabbled {targetLetters|-} <dictionaryFile>" << endl << endl
		<< " e.g,, " << endl
		<< "\t./scrabbled bro ~/mydictionary.txt" << endl
		<< "\t2  or" << endl
		<< "\t5  orb" << endl
		<< "\t5  rob" << endl << endl
		<< "\t./scrabbled br. ~/mydictionary.txt" << endl
		<< "\t1  Dr" << endl
		<< "\t1  Mr" << endl
		<< "\t1  Or" << endl
		<< "\t4  Orb" << endl
		<< "\t4  bAr" << endl
		<< "\t3  bE" << endl
		<< "\t4  bUr" << endl
		<< "\t3  bY" << endl
		<< "\t4  brA" << endl
		<< "\t1  rE" << endl
		<< "\t4  rIb" << endl
		<< "\t4  rOb" << endl
		<< "\t4  rUb" << endl << endl
		<< "\t(- four highest scoring alternatives -)" << endl
		<< "\t./scrabbled poraens. ~/mydictionary.txt|sort -r -n |head -4|perl -pe \"s/\\w+\\s+(.*)$/\\1/\"" << endl
		<< "\tpersonaL" << endl
		<< "\tpersona" << endl
		<< "\tpErsonae" << endl
		<< "\toperanDs" << endl

		<< endl << endl;
}

int main(int argc, char** argv)
{
	if (3 == argc) {
		string letters;
		if (strcmp("-", argv[1]) == 0)
			getline(cin, letters);
		else
			letters = argv[1];
		return showScrabbleWords(letters, string(argv[2]));
	}
	string targetWord = "";
	if (1 == argc || (argc == 2 && 0 == strcmp("-h", argv[1]))) {
		showHelp();
		return 1;
	}
	// hidden function: print internal 'key' associated with given targetLetters
	if (argc == 2) {
		targetWord = argv[1];
		for (auto& c : targetWord)
			c = tolower(c);
		sort(targetWord.begin(), targetWord.end());
		cout << endl << targetWord << endl;
	}
	return 0;
}