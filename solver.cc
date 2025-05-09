#include <iostream>
#include <assert.h>

#include <vector>
#include <string>
#include <sstream>
#include <fstream>

using namespace std;

// https://www.geeksforgeeks.org/how-to-change-console-color-in-cpp/
#define BLACK_F "30"
#define RED_F "31"
#define GREEN_F "32"
#define YELLOW_F "33"
#define BLUE_F "94"
#define CYAN_F "96"

#define WHITE_B "107"
#define RED_B "101"
#define GREEN_B "102"
#define YELLOW_B "43"
#define CYAN_B "106"
#define DEFAULT "0"
#define OUTPUTCOLOR(clr) cout << "\033[" clr "m";

#define BACKGROUND(clr) cout << "\033[" BLACK_F "m" << "\033[" clr "m";

#define STOPTEST false
#define DEBUG_PAUSE_AFTER_SHOW false
//#define DEBUG_LEFTMOST
//#define DEBUG_CHECKLINE
//#define SCANNING_DEBUG
//#define DEBUG


struct ParamBundle{
	bool pauseEverySemiIter;
	bool pauseEverySemiIter_evenDuringGuesses;
	bool pauseEveryGuess;
	bool doCutNumbers;
	bool checkAmbiguity;
	bool exploreSolvingGuesses;
	bool printEverySemiIter;
	bool printResultOfLineSolve;
	bool printRecursiveStructure;
	bool printGuessStructure;
	bool printHypPreview;
	bool printHypGrid;
};
ParamBundle GlobalParams;
void setGlobalParameters(){
	// TODO Check application of those parameters
	GlobalParams.pauseEverySemiIter						= false;
	GlobalParams.pauseEverySemiIter_evenDuringGuesses	= false		and GlobalParams.pauseEverySemiIter;
	GlobalParams.pauseEveryGuess						= false;

	GlobalParams.checkAmbiguity							= true;
	GlobalParams.exploreSolvingGuesses					= true		and GlobalParams.pauseEverySemiIter;

	GlobalParams.doCutNumbers							= true;
	
	GlobalParams.printEverySemiIter						= false;
	GlobalParams.printResultOfLineSolve					= false;

	GlobalParams.printHypPreview						= false;
	GlobalParams.printHypGrid							= false 	and GlobalParams.printHypPreview;

	GlobalParams.printRecursiveStructure				= false;
	GlobalParams.printGuessStructure					= false		or GlobalParams.printRecursiveStructure;
}



#define PRINTONENUMBER(n, back) BACKGROUND(back) \
								printOneNumber(n);

void printOneNumber(int n){
	if (999 < n){
		cout << "\033[31m" << "*";
	} else if (100 <= n){
		cout << "\033[93m" << n/100;
	} else {
		switch (n/10){
			case(0):
				OUTPUTCOLOR(DEFAULT);
				break;
			case(1):
				cout << "\033[33m";
				break;
			case(2):
				cout << "\033[32m";
				break;
			case(3):
				cout << "\033[36m";
				break;
			case(4):
				cout << "\033[96m";
				break;
			case(5):
				cout << "\033[34m";
				break;
			case(6):
				cout << "\033[94m";
				break;
			case(7):
				cout << "\033[35m";
				break;
			case(8):
				cout << "\033[95m";
				break;
			case(9):
				cout << "\033[90m";
				break;
		}
		cout << n%10;
	}
	OUTPUTCOLOR(DEFAULT)
	return;
}

void explainColors(){
	cout << "This program often needs to print a big number in only one character." << endl;
	cout << "The color and value of the character will depend on the number according to the list below." << endl;

	cout << "\033[37m" << "00x" << "  ";
	cout << "\033[33m" << "01x" << "  ";
	cout << "\033[32m" << "02x" << "  ";
	cout << "\033[36m" << "03x" << "  ";
	cout << "\033[96m" << "04x" << "  ";
	cout << "\033[34m" << "05x" << "  ";
	cout << "\033[94m" << "06x" << "  ";
	cout << "\033[35m" << "07x" << "  ";
	cout << "\033[95m" << "08x" << "  ";
	cout << "\033[90m" << "09x";
	OUTPUTCOLOR(DEFAULT)
	cout << "  for number below 100." << endl;

	cout << "\033[93m" << "x--";
	OUTPUTCOLOR(DEFAULT)
	cout << "  for hundreds and ";
	cout << "\033[31m" << "*";
	OUTPUTCOLOR(DEFAULT)
	cout << "  for numbers above 999." << endl;
	cout << "Examples:" << endl;
	cout << endl << "The numbers {00, 11, 22, ..., 99, 110, 121} will be:" << endl;
	for (int n(0); n < 130; n+=11){
		printOneNumber(n);
	}
	cout << endl << "The numbers {64, 128, 256, 512, 1024} will be:" << endl;
	for (int n(64); n < 2000; n*=2){
		printOneNumber(n);
	}
	
	cout << endl << endl;
}

void cheatSheetColors(){
	cout << "\033[37m" << "0x" << " ";
	cout << "\033[33m" << "1x" << " ";
	cout << "\033[32m" << "2x" << " ";
	cout << "\033[36m" << "3x" << " ";
	cout << "\033[96m" << "4x" << " ";
	cout << "\033[34m" << "5x" << " ";
	cout << "\033[94m" << "6x" << " ";
	cout << "\033[35m" << "7x" << " ";
	cout << "\033[95m" << "8x" << " ";
	cout << "\033[90m" << "9x" << " ";
	cout << "\033[93m" << "x--" << " ";
	cout << "\033[31m" << "*" << endl;
	OUTPUTCOLOR(DEFAULT)
}


struct gridPTR{
	size_t i;
	size_t j;
};

enum cell {Void, Full, Empt};
enum flagOu_t {NoSol, Stuck, Unique};

struct linSolvOu_t{
	flagOu_t flag;
	unsigned int modifCells;
};

std::ostream& operator<<(std::ostream& out, const cell& v){
	switch (v){
		case Full:
			//return out << '#';
			//return out << "\u25A0";  //'#';
			return out << "\u25AE";  //'#';
		case Empt:
			return out << 'x'; // '-'
		case Void:
#ifdef DEBUG
			return out << '\'';
#else
			return out << ' '; // '_'
#endif
		default:
			return out << '?';
	}
}

std::ostream& operator<<(std::ostream& out, const flagOu_t& f){
	switch (f){
		case NoSol:
			return out << "NoSol";
		case Stuck:
			return out << "Stuck"; // '-'
		case Unique:
			return out << "Unique";
		default:
			return out << "??";
	}
}


typedef string string;
typedef vector<cell> line_t;
typedef vector<line_t> tab_t;


class Blocks{
public:
	std::vector<size_t> v;

	Blocks()
	:v({}){}

	Blocks(std::vector<size_t> v_, bool reverse = false)
	:v(v_){
		if (reverse){
			size_t n(v_.size());
			for (size_t i(0); i < n; i++) v[n - i - 1] = v_[i];
		}
	}

	unsigned int init(string line){
		unsigned int totVal(0);
		std::stringstream stream(line);
		int n;
		while(stream >> n) {
			totVal += n;
			v.push_back(n);
		}
		return totVal;
	}
};

struct GivenBlock{
	size_t leftMost;
	size_t rightMost;
};

void printScanning(size_t potStart, size_t scaner, line_t lin, bool blocking = false){
	cout << "\"";
	BACKGROUND(RED_B)
	for (size_t i(0); i < lin.size(); i++){
		if (i==potStart) BACKGROUND(GREEN_B)
		if (i==scaner)  OUTPUTCOLOR(DEFAULT)
		cout << lin[i];
	}
	OUTPUTCOLOR(DEFAULT)
	cout << "\"" << endl;

	if (DEBUG_PAUSE_AFTER_SHOW and blocking){
		string temp;
		cout << ">> ";
		cin >> temp;
	}
}


bool resetScanning(size_t idx, vector<size_t> const& res, Blocks const& blocks, line_t const& lin, size_t& scaner, size_t&potStart){
	// Reset potStart and scaner to both look at right-most square of left-most position of block idx, + 1 for spacing.
	if (idx > 0){
		scaner = std::max(res[idx], res[idx-1] + blocks.v[idx-1] + 1);
	} else {
		scaner = res[idx];
	}
	potStart = scaner;
	while (potStart < lin.size() and lin[potStart] == Empt) potStart++;
	if (potStart == lin.size()) {
		/*
OUTPUTCOLOR(RED_F)
		cout << "ERROR"; OUTPUTCOLOR(DEFAULT)
		cout << " - during resetScanning - (potStart == n)" << endl;
		*/
		return true;
	}
	scaner = potStart;
	return false;
}

bool leftMostConfiguration(Blocks const& blocks, line_t const& lin, vector<size_t>& res, bool realExploration = true){
	size_t n(lin.size());
	size_t k(blocks.v.size());

#ifdef DEBUG_LEFTMOST
	cout << endl << "Calling leftMostConfiguration on" << endl << "\"";
	for (auto const& val:lin) cout << val;
	cout << "\" with blocks [";
	for (auto const& val:blocks.v) cout << val << " ";
	cout << "]." << endl;
#endif

	/* === Reading Given-Blocks === */
	vector<GivenBlock> givenBlocks({}); // left to right, i.e. increasing
	size_t scaner(0);
	while (scaner < n){
		while (scaner < n and lin[scaner] != Full) scaner++; // Until scaner points to left-most Full of givenBlock
		if (scaner < n) {
			GivenBlock givenBlock({scaner, 0});
			while (scaner+1 < n and lin[scaner+1] == Full) scaner++; // Until scaner points to left-most Full og givenBlock
			givenBlock.rightMost = scaner;
			givenBlocks.push_back(givenBlock);
			scaner++;
		}
	}
	size_t nb_givenBlocks(givenBlocks.size());

#ifdef DEBUG_LEFTMOST
	cout << "Right-most of given blocks:";
	for (auto const& block:givenBlocks) cout << block.leftMost << "-" << block.rightMost << "; ";
	cout << endl;
#endif

	/* === Sliding Blocks === */
	size_t potStart(0);
	bool notAllBlocks(true);
	bool notCoveredAllGB(true);
	bool changedBlock(false);

	while (notAllBlocks or notCoveredAllGB){

		/* === Covering Given-Blocks === */
		size_t givB_Idx(nb_givenBlocks - 1);
		size_t idx(k-1);
		size_t startingIdx(0);
		changedBlock = false;
		while (not changedBlock and givB_Idx < nb_givenBlocks) {
			// Move the blocks from the rightest of our left to the right until being covered
			//cout << "Covering gb " << givB_Idx << "  (" << << givenBlocks[givB_Idx].leftMost << "-" << givenBlocks[givB_Idx].rightMost << ")" << endl;
			while (givenBlocks[givB_Idx].leftMost < res[idx]) idx--; // too far already
			if (k < idx){
				if (realExploration){
					OUTPUTCOLOR(RED_F)
					cout << "Error!" << endl;
					OUTPUTCOLOR(DEFAULT)
					cout << " Can't cover with any blocks" << endl;
				}
				return false;
			}
			if (blocks.v[idx] < givenBlocks[givB_Idx].rightMost - givenBlocks[givB_Idx].leftMost + 1){
				res[idx] = givenBlocks[givB_Idx].rightMost + 1;
			} else {
				if (givenBlocks[givB_Idx].rightMost <= res[idx] + blocks.v[idx] - 1){
				} else {
					res[idx] = givenBlocks[givB_Idx].rightMost + 1 - blocks.v[idx];
					changedBlock = true;
				}
				givB_Idx--;
			}
		}
		if (changedBlock){
			startingIdx = idx;
			notAllBlocks = true;
		} else {
			notCoveredAllGB = false;
		}

		/*
		cout << endl << endl;
		for (size_t tempIdx(0); tempIdx < res.size(); tempIdx++) cout << res[tempIdx] << " ";
		cout << endl;
		*/

		for (size_t idx(startingIdx); idx < k; idx++){

			bool errorFlag(resetScanning(idx, res, blocks, lin, scaner, potStart));
			if (errorFlag){
				if (realExploration){
					OUTPUTCOLOR(RED_F)
					cout << "ERROR"; OUTPUTCOLOR(DEFAULT)
					cout << " - resetScanning before Emptiness" << endl;
				}
				return false;
			}

			#ifdef SCANNING_DEBUG
			cout << "looking for block " << idx << " of length " << blocks.v[idx] << endl;
			printScanning(potStart, scaner, lin);
			#endif

			while (scaner < n and scaner - potStart < blocks.v[idx]){

				// can't fit block idx, need to jump after Emptiness
				if (lin[scaner] == Empt){
					if (res[idx] != scaner){
						res[idx] = scaner;
						notCoveredAllGB = true;
					}
					bool errorFlag(resetScanning(idx, res, blocks, lin, scaner, potStart));
					if (errorFlag){
						if (realExploration){
							OUTPUTCOLOR(RED_F)
							cout << "ERROR"; OUTPUTCOLOR(DEFAULT)
							cout << " - resetScanning after Emptiness" << endl;
						}
						return false;
					}

					#ifdef SCANNING_DEBUG
					cout << "Scanning - after empty-jumping" << endl;
					printScanning(potStart, scaner, lin);
					#endif
				}

				// maybe we can fit, at least we can increase
				scaner++;
				#ifdef SCANNING_DEBUG
				cout << "Scanning - extending:" << scaner << endl;
				printScanning(potStart, scaner, lin);
				#endif


				// we have enough size, let's check extremes
				if (scaner - potStart == blocks.v[idx] and (scaner < n and lin[scaner] == Full)){
					potStart++;
					while (potStart < n and lin[potStart - 1] == Full) potStart++;
					if (potStart == n){
						if (realExploration){
							OUTPUTCOLOR(RED_F)
							cout << "ERROR"; OUTPUTCOLOR(DEFAULT)
							cout << " - after getting pulled - potStart == n" << endl;
						}
						return false;
					}

					#ifdef SCANNING_DEBUG
					cout << "Scanning - getting pulled" << endl;
					printScanning(potStart, scaner, lin);
					#endif

				}
			}
			#ifdef SCANNING_DEBUG
			cout << "DBG - scaner=" << scaner << "; potStart=" << potStart << "; blocks.v[" << idx << "]=" << blocks.v[idx] << endl;
			#endif
			if (scaner - potStart < blocks.v[idx]){
				if (realExploration){
					OUTPUTCOLOR(RED_F)
					cout << "ERROR"; OUTPUTCOLOR(DEFAULT)
					cout << " - during sliding - (not enough space)" << endl;
				}
				return false;
			}
			// We can fit block[idx] in [potStart, scaner[
			//cout << "can fit block " << idx << " in [" << potStart << ", " << scaner << "[" << endl;
			if (res[idx] != potStart){
				res[idx] = potStart;
				notCoveredAllGB = true;
			}
		}


#ifdef SCANNING_DEBUG
		cout << "after scanning from " << startingIdx << endl;
		for (size_t tempIdx(0); tempIdx < res.size(); tempIdx++) cout << res[tempIdx] << " ";
		cout << endl;
#endif

		if (startingIdx == 0){
			notAllBlocks = false;
		}
	}
#ifdef DEBUG_LEFTMOST
	cout << endl << "Result:";
	for (auto const& val:res) cout << " " << val;
	cout << "." << endl;
#endif

	return true;
}

bool rightMostConfiguration(Blocks const& blocks, line_t const& lin, vector<size_t>& res, bool realExploration = true){
	size_t n(lin.size());
	line_t lin_inv(n, Void);
	for (size_t i(0); i < n; i++) lin_inv[n - i - 1] = lin[i];

	Blocks blocks_inv(blocks.v, true);

	OUTPUTCOLOR(YELLOW_F)
	vector<size_t> inverted(blocks.v.size(), 0);
	bool flag(leftMostConfiguration(blocks_inv, lin_inv, inverted, realExploration));
	if (not flag) return false;
	OUTPUTCOLOR(DEFAULT)
	size_t k(inverted.size());

	for (size_t i(0); i < k; i++) res[k - i - 1] = n - inverted[i] - blocks_inv.v[i];

	return true;
}

bool checkLine_simple(Blocks const& blocks, line_t const& lin, line_t& res, bool realExploration = true){
	size_t n(lin.size());
	size_t k(blocks.v.size());

	// Copy lin (input) into res (output)
	for (size_t i(0); i < n; i++){
		if (lin[i] != Void) res[i] = lin[i];
	}

	#ifdef DEBUG_CHECKLINE
		if (realExploration){
			cout << endl;
			cout << "Calling checkLine_simple on" << endl;
			cout << "\"";
			for (auto const& val:lin) cout << val;
			cout << "\" with blocks [";
			for (auto const& val:blocks.v) cout << val << " ";
			cout << "] gives :" << endl;
		}
	#endif

	// Find LEFT-most configuration
	vector<size_t> leftMost(blocks.v.size(), 0); // leftMost[i] = left-most square of the i-th block in the left-most configuration
	bool flag ( leftMostConfiguration(blocks, lin, leftMost, realExploration));
	if (not flag) return false;

	// Find RIGHT-most configuration
	vector<size_t> rightMost(blocks.v.size(), 0); // rightMost[i] = right-most square of the i-th block in the right-most configuration
	flag = (rightMostConfiguration(blocks, lin, rightMost, realExploration));
	if (not flag) return false;

	#ifdef SCANNING_DEBUG
		if (realExploration){
			cout << endl << endl;
			cout << "Continuing checkLine_simple on" << endl << "\"";
			for (auto const& val:lin) cout << val;
			cout << "\" with blocks [";
			for (auto const& val:blocks.v) cout << val << " ";
			cout << "]." << endl;
			cout << endl;
		}
	#endif


	for (size_t idx(0); idx < k; idx++){
		#ifdef DEBUG_CHECKLINE
		if (realExploration){
			cout << endl;
			cout << "DBG - checkLine_simple - block " << idx << " of length " << blocks.v[idx] << endl;
			cout << "[" << leftMost[idx] << ", " << rightMost[idx] << "]" << endl;
		}
		#endif


		// draw intersection of both extremes if exists
		for (size_t pos(rightMost[idx]); pos < leftMost[idx] + blocks.v[idx]; pos++) res[pos] = Full;


		// slide block from left-most to right-most and mark only possible slots
		size_t potStart(leftMost[idx]);
		size_t scaner(leftMost[idx]);
		while (potStart <= rightMost[idx]){
			#ifdef SCANNING_DEBUG
			if (realExploration){
				cout << "Maybying - Start" << endl;
				printScanning(potStart, scaner, res);
			}
			#endif

			while (scaner < potStart + blocks.v[idx] and lin[scaner] != Empt){
				scaner++;
			}

			#ifdef SCANNING_DEBUG
			if (realExploration){
				cout << "Maybying - foundEnd" << endl;
				printScanning(potStart, scaner, res);
			}
			#endif

			if (scaner == potStart + blocks.v[idx]){
				if ((scaner == n or lin[scaner] != Full) and (potStart == 0 or lin[potStart-1] != Full)){
					for (size_t pos(potStart); pos < scaner; pos++){
						if (res[pos] != Full) res[pos] = Void;
					}
					#if defined(SCANNING_DEBUG) || defined(DEBUG_CHECKLINE)
						if (realExploration){
							cout << "Maybying - foundEnd - valid : erase x" << endl;
							printScanning(potStart, scaner, res);
						}
					#endif
				}
				potStart++;
			} else if (scaner < potStart + blocks.v[idx]){
				while (lin[scaner] == Empt) scaner++;
				potStart = scaner;
			}

			#ifdef SCANNING_DEBUG
			if (realExploration){
				cout << "Maybying - End" << endl;
				printScanning(potStart, scaner, res, true);
			}
			#endif
		}
	}
#ifdef DEBUG_CHECKLINE
		cout << "\"";
		for (auto const& val:res) cout << val;
		cout << "\"" << endl << endl;
#endif

	return true;
}

bool guessLine(Blocks const& blocks, line_t const& lin, size_t idx, cell guess){
	// Call checkLine on a copy of LIN where the cell idx is replaced by guess.
	// Returns TRUE if everything seems fine and FALSE if the guess gives an error.

#ifdef DEBUG_CHECKLINE
		cout << endl;
		cout << "Calling guessLine - idx=" << idx << " with guess= " << guess << endl;
#endif

	size_t n(lin.size());
	line_t lin_changed(n, Empt);
	line_t out(n, Empt);
	for (size_t i(0); i < n; i++){
		if (i != idx){
			lin_changed[i] = lin[i];
		} else {
			assert(lin[i] == Void);
			lin_changed[i] = guess;
		}
	}
	return checkLine_simple(blocks, lin_changed, out, false);
}


bool checkLine(Blocks const& blocks, line_t const& lin, line_t& res){
	size_t n(lin.size());

	line_t lin_changed(n, Empt);
	for (size_t i(0); i < n; i++){
		lin_changed[i] = lin[i];
	}

	bool needToSolve(true);
	while (needToSolve){
		bool isOkey(checkLine_simple(blocks, lin_changed, res));
		if (not isOkey) return false;

		needToSolve = false;

		for (size_t i(0); i < n; i++){
			if (res[i] == Void){
				bool FullIsOkey(guessLine(blocks, lin_changed, i, Full));
				bool EmptIsOkey(guessLine(blocks, lin_changed, i, Empt));
				if (not FullIsOkey){
					lin_changed[i] = Empt;
					needToSolve = true;
				}
				if (not EmptIsOkey){
					lin_changed[i] = Full;
					needToSolve = true;
				}
			}
		}
	}
	return true;
}

class State{
private:
	size_t n; // n rows
	size_t m; // m columns
	size_t count;
	vector<Blocks> const& BlocksInRow;
	vector<Blocks> const&  BlocksInCol;
	bool rowsAreChecked;
	bool colsAreChecked;

	tab_t tab;
	vector<size_t> countInRow;
	vector<bool> modifRow;
	vector<bool> modifCol;
public:
	State(size_t n_, size_t m_, vector<Blocks> const& BlocksInRow_, vector<Blocks> const& BlocksInCol_, bool rowChecked = false, bool colChecked = false)
	:n(n_), m(m_), count(0), BlocksInRow(BlocksInRow_), BlocksInCol(BlocksInCol_), rowsAreChecked(rowChecked), colsAreChecked(colChecked)
	{
		tab.resize(n);
		countInRow.resize(n);
		modifRow.resize(n);
		modifCol.resize(m);
		for (size_t i(0); i < n; i++){
			tab[i].resize(m);
			modifRow[i] = true;
		}
		for (size_t i(0); i < m; i++){
			modifCol[i] = true;
		}
	}

	State* deepCopy(){
		State* other(new State(n, m, BlocksInRow, BlocksInCol, rowsAreChecked, colsAreChecked));
		other->tab = tab;
		other->count = count;
		other->countInRow = countInRow;
		other->modifRow = modifRow;
		other->modifCol = modifCol;
		return other;
	}

	cell get(size_t i, size_t j) const{ return tab[i][j];}
	
	void play(size_t i, size_t j, cell val){
		assert(i < n); assert(j < m); assert(val != Void);
		if (tab[i][j] == Void){
			tab[i][j] = val;
			modifRow[i] = true;
			rowsAreChecked = false;
			modifCol[j] = true;
			colsAreChecked = false;
			countInRow[i]++;
			count++;
		} else {
			cout << "ERROR - playing at non-void" << endl;
			throw;
		}
	}

	size_t getCount() const{
		return count;
	}

	bool isSolved() const{
		return count == n*m;
	}

	bool needRowCheck() const{return !rowsAreChecked;}
	bool needColCheck() const{return !colsAreChecked;}
	void notifyRowsChecked(){rowsAreChecked = true;}
	void notifyColsChecked(){colsAreChecked = true;}

	void print(bool pretty = false, gridPTR highlight = {(unsigned int)(-1), (unsigned int)(-1)}) const{
		cout << "┌";
		for (size_t i(0); i<m; i++) cout << "─";
		if (rowsAreChecked) OUTPUTCOLOR(GREEN_F);
		cout << "┐";
		OUTPUTCOLOR(DEFAULT);
		cout << endl;

		for (size_t i(0); i < n; i++){
			cout << "│";
			for (size_t j(0); j < m; j++){
				if (pretty){
					switch (tab[i][j]){
						case Full:
							BACKGROUND(WHITE_B);
						default:
							cout << " ";
							BACKGROUND(DEFAULT);
					}
				} else {
					if ((i+j)%2 == 0) cout << "\e[40m";
					if (highlight.i == i and highlight.j == j) BACKGROUND(GREEN_B)
					cout << tab[i][j];
					BACKGROUND(DEFAULT);
				}
			}
			if (not modifRow[i]) OUTPUTCOLOR(GREEN_F);
			cout << "│";
			OUTPUTCOLOR(DEFAULT);
			if (!GlobalParams.doCutNumbers){
				for (auto const& val: BlocksInRow[i].v) cout << " " << val;
			}
			cout << endl;
		}

		if (colsAreChecked) OUTPUTCOLOR(GREEN_F);
		cout << "└";
		OUTPUTCOLOR(DEFAULT);

		for (size_t i(0); i<m; i++){
			if (not modifCol[i]) OUTPUTCOLOR(GREEN_F);
			cout << "─";
			OUTPUTCOLOR(DEFAULT);
		}
		if (rowsAreChecked and colsAreChecked) OUTPUTCOLOR(GREEN_F);
		cout << "┘";
		OUTPUTCOLOR(DEFAULT);
		cout << endl;


		if (!GlobalParams.doCutNumbers){
			bool stillColPrinting(true);
			for (size_t j(0); stillColPrinting; j++){
				stillColPrinting = false;
				cout << " ";
				for (size_t i(0); i<m; i++){
					if (j < BlocksInCol[i].v.size()){
						if (j +1 < BlocksInCol[i].v.size()) stillColPrinting = true;
						printOneNumber(BlocksInCol[i].v[j]);
					} else {
						cout << " ";
					}
				}
				cout << " " << endl;
			}
		}

		cout << endl;
	}

	int checkRow(Blocks const& blocks, size_t i){
		// return {1} if modified something {0} if modified nothing and {-1} for errors.

		 // if modifRow[i] == False, we do nothing.
		if (not modifRow[i]) return 0;
		modifRow[i] = false; // Otherwise, we set it to false for detecting changes.

		line_t lin;
		lin.resize(m);
		for (size_t k(0); k < m; k++) lin[k] = tab[i][k];

		line_t out(m, Empt);
		bool succesFlag(checkLine(blocks, lin, out));
		if (not succesFlag) return -1;

		for (size_t k(0); k < m; k++){
			if (out[k] != Void and out[k] != lin[k]) play(i, k, out[k]);
		}
		
		bool flag(modifRow[i]); // if modifRow has been set to true (by a modification), then we return true.
		modifRow[i] = false;
		if (flag) return 1;
		return 0;
	}

	int checkCol(Blocks const& blocks, size_t i){
		// return true of modified something.
		if (not modifCol[i]) {
			return 0;
		}
		modifCol[i] = false;

		line_t lin;
		lin.resize(n);
		for (size_t k(0); k < n; k++) lin[k] = tab[k][i];

		line_t out(n, Empt);
		bool succesFlag(checkLine(blocks, lin, out));
		if (not succesFlag) return -1;

		for (size_t k(0); k < n; k++){
			if (out[k] != Void and out[k] != lin[k]) play(k, i, out[k]);
		}

		bool flag(modifCol[i]); // if modifRow has been set to true (by a modification), then we return true.
		modifCol[i] = false;
		if (flag) return 1;
		return 0;
	}
};

vector<State*> GLOBAL_SOLUTIONS;

bool checkEntryAndFF(){
	cout << "(or write down \"f\" in order to fast forward)" << endl;
	switch(cin.get()){
		case '\n':
			return false;
		case 'f':
			cout << "Fast-forwarding..." << endl;
			while(cin.get() != '\n');
			return true;
		default:
			cout << "Unkown command! Ignoring..." << endl;
			while(cin.get() != '\n');
			return false;
	}
	return false;
}

class LinSolver{
public:
	State* state = nullptr;
private:
	size_t n;
	size_t m;
	vector<Blocks> const& BlocksInRow;
	vector<Blocks> const&  BlocksInCol;
public:
	LinSolver(size_t n_, size_t m_, vector<Blocks> const& rowBlocks, vector<Blocks> const& colBlocks)
	:n(n_), m(m_), BlocksInRow(rowBlocks), BlocksInCol(colBlocks)
	{
		state = new State(n, m, BlocksInRow, BlocksInCol);
	}

	~LinSolver(){
		delete(state);
	}

	LinSolver* copy(){
		LinSolver* other(new LinSolver(n, m, BlocksInRow, BlocksInCol));
		other->state = state->deepCopy();
		return other;
	}

	LinSolver* copy(gridPTR hyp, cell newVal){
		LinSolver* other(copy());
		other->change(hyp.i, hyp.j, newVal);
		return other;
	}

	void print(bool pretty = false, gridPTR highlight = {(unsigned int)(-1), (unsigned int)(-1)}){
		state->print(pretty, highlight);
	}

	linSolvOu_t lineSolve(size_t depth = 0, bool trueExploration = true){
		size_t filledBefore(state->getCount());
		bool fastForward(not GlobalParams.pauseEverySemiIter);
		while (state->needRowCheck() or state->needColCheck()){

			if (state->needRowCheck()){

				if (GlobalParams.printEverySemiIter) print();
				if (((GlobalParams.pauseEverySemiIter and depth == 0)
					 or (GlobalParams.pauseEverySemiIter_evenDuringGuesses))
					and (not fastForward)){
					cout << "Press Enter to check rows" << endl;
					fastForward = checkEntryAndFF();
				}

				for (size_t i(0); i < n; i++){
					int flag(state->checkRow(BlocksInRow[i], i));
					if (flag < 0) return {NoSol, (unsigned int)(state->getCount() - filledBefore)}; // TODO - return an indicator of the row/col which is erroneous
				}
				state->notifyRowsChecked();
			}

			if (state->needColCheck()){
				if (GlobalParams.printEverySemiIter) print();
				if (((GlobalParams.pauseEverySemiIter and depth == 0)
					 or (GlobalParams.pauseEverySemiIter_evenDuringGuesses))
					and (not fastForward)){
					cout << "Press Enter to check columns" << endl;
					fastForward = checkEntryAndFF();
				}

				for (size_t i(0); i < m; i++){
					int flag(state->checkCol(BlocksInCol[i], i));
					if (flag < 0) return {NoSol, (unsigned int)(state->getCount() - filledBefore)};
				}
				state->notifyColsChecked();
			}
		}

		// Either LinSolve is Stuck or it has found a Unique solution.

		if (GlobalParams.printResultOfLineSolve) print();
		if (state->isSolved()){
			if (trueExploration) GLOBAL_SOLUTIONS.push_back(state->deepCopy());
			return {Unique, (unsigned int)(state->getCount() - filledBefore)};
		}
		return {Stuck,  (unsigned int)(state->getCount() - filledBefore)};
	}

	void change(size_t i, size_t j, cell newV){
		state->play(i, j, newV);
	}
};

struct HypHolder{
	unsigned int n_main;
	unsigned int n_sec;
	size_t i;
	size_t j;
	cell best;
};

bool operator<(HypHolder const& l, HypHolder const&r){
	return ((l.n_main < r.n_main) or ((l.n_main == r.n_main) and (l.n_sec < r.n_sec)));
}

typedef vector<HypHolder*> ListHH;

class GridHH{
private:
	size_t n;
	size_t m;
	vector<vector<HypHolder*>> grid;
	gridPTR lastHyp; //stores the last hypothesis.
	bool haveResetSinceChange;

	void myFreeLastHyp(){
		if (grid[lastHyp.i][lastHyp.j] != nullptr){
			delete(grid[lastHyp.i][lastHyp.j]);
			grid[lastHyp.i][lastHyp.j] = nullptr;
		}
	}

	bool advance(){
		lastHyp.i++;
		if (lastHyp.i == n){
			lastHyp.i = 0;
			lastHyp.j++;
			if (lastHyp.j == m){
				if (haveResetSinceChange) return false;
				haveResetSinceChange = true;
				lastHyp.j = 0;
			}
		}
		return true;
	}

public:
	GridHH(size_t n_, size_t m_)
	:n(n_), m(m_), lastHyp({n-1, m-1}), haveResetSinceChange(false){
		grid.resize(n);
		for (auto& line:grid){
			line.resize(m);
		}
	}

	bool get(State* state, gridPTR& output){
		// Upon request, it advances the LastHypothesis to the next useful hypothesis.
		if (not advance()) return false;
		while (state->get(lastHyp.i, lastHyp.j) != Void ){
			set(nullptr);
			if (not advance()) return false;
		}
		output = lastHyp;
		return true;
	}

	void set(HypHolder* hh){
		myFreeLastHyp();
		grid[lastHyp.i][lastHyp.j] = hh;
	}

	void notifyChange(){
		// Make sure we will at least cycle fully once before stopping.
		haveResetSinceChange = false;
	}

	void print(){
		cheatSheetColors();
		cout << "┌";
		for (size_t i(0); i<m; i++) cout << "─";
		cout << "┐" << endl;

		for (size_t i(0); i < n; i++){
			cout << "│";
			for (size_t j(0); j < m; j++){
				if (lastHyp.i == i and lastHyp.j == j) BACKGROUND(GREEN_B)
				if (grid[i][j] != nullptr){
					printOneNumber(grid[i][j]->n_sec);
				} else {
					cout << "-";
				}
				BACKGROUND(DEFAULT);
			}
		cout << "│";
		cout << endl;
		}
		cout << "└";
		for (size_t i(0); i<m; i++){
			cout << "─";
		}
		cout << "┘" << endl;
		return;
	}

	HypHolder getBest(){
		cout << "ERROR - TODO - implement sorting" << endl;
		cout << "i j:" << " ";
		size_t i, j;
		cin >> i >> j;

		return *grid[i][j];
	}
};


class Spacer{
public:
	size_t n;

	Spacer(size_t depth)
	:n(depth){}
};

ostream& operator<<(ostream& out, const Spacer& s){
	out << s.n;
	for (size_t i(0); i < s.n; i++) out << " ";
	out << "* ";
	return out;
}

class ComplexSolver;

class ComplexSolver{
public:
	LinSolver* Lsolver = nullptr;
private:
	size_t n;
	size_t m;
	vector<Blocks> const& BlocksInRow;
	vector<Blocks> const&  BlocksInCol;
	GridHH* grid;
	size_t depth;
	Spacer space;
public:
	ComplexSolver(size_t n_, size_t m_, vector<Blocks> const& rowBlocks, vector<Blocks> const& colBlocks, size_t depth_ = 0)
	:n(n_), m(m_), BlocksInRow(rowBlocks), BlocksInCol(colBlocks), depth(depth_), space(depth_)
	{
		Lsolver = new LinSolver(n, m, BlocksInRow, BlocksInCol);
		grid = new GridHH(n, m);
	}

	ComplexSolver(const ComplexSolver* ptr, size_t i, size_t j, cell toPlay)
	:n(ptr->n), m(ptr->m), BlocksInRow(ptr->BlocksInRow), BlocksInCol(ptr->BlocksInCol), depth(ptr->depth + 1), space(ptr->depth + 1)
	{
		Lsolver = ptr->Lsolver->copy({i, j}, toPlay);
		grid = new GridHH(n, m);
	}


	unsigned int complexSolve(){
		unsigned int nSol(0);
		bool fastForward(false);
		bool justStuckStuck(false);
		do {
			linSolvOu_t out(Lsolver->lineSolve((depth == 0) and not justStuckStuck and not fastForward));
			if (0 < out.modifCells){
				fastForward = false;
				grid->notifyChange();
			}
			justStuckStuck = false;

			switch (out.flag){
				case Unique:
					if (GlobalParams.printGuessStructure){
						cout << space << "A unique solution has been found:" << endl;
						Lsolver->print(false);
					}
					return nSol+1;
				case NoSol:
					if (GlobalParams.printGuessStructure){
						cout << space << "No valid solution - impossible problem" << endl;
					}
					return nSol+0;
				case Stuck:
					if (GlobalParams.printGuessStructure){
						cout << endl;
						cout << space << "The simple lin-solver is ";
						BACKGROUND(RED_B)
						cout << "stuck";
						BACKGROUND(DEFAULT)
						cout << ":" << endl;
					}
			}

			gridPTR hyp({n, m});
			bool validHypFlag(grid->get(Lsolver->state, hyp));
			if (GlobalParams.printHypGrid) grid->print();
			if (GlobalParams.printHypPreview) Lsolver->print(false, hyp);

			if (validHypFlag){
				if (GlobalParams.printGuessStructure) cout << space << "hyp on (" << hyp.i << ", " << hyp.j << ")" << endl;
				if (GlobalParams.pauseEveryGuess and not fastForward){
					cout << space << "Press Enter to continue." << endl;
					fastForward = checkEntryAndFF();
				}

				LinSolver* hyp_full(nullptr);
				LinSolver* hyp_empt(nullptr);
				linSolvOu_t out_full;
				linSolvOu_t out_empt;

				for (cell filling(Full); filling != Void; filling = (filling == Full ? Empt : Void)){
					LinSolver* hypSolver(Lsolver->copy(hyp, filling));
					linSolvOu_t hypOut(hypSolver->lineSolve(false));
					if (GlobalParams.printGuessStructure and 0 < hypOut.modifCells){
						if (hypOut.flag == Unique) {
						cout << "The solver just found out that the guess ";
						cout << filling;
						cout << " on (" << hyp.i << ", " << hyp.j << ") gives a solution." << endl;
						} else {
							hypSolver->print(false);
						}
					}
					if (GlobalParams.printGuessStructure){
						cout << space;
						cout << filling;
						cout << " --> (" << hypOut.flag << ", " << hypOut.modifCells << ")" << endl;
					}
					if (hypOut.flag == Unique){
						nSol++;

						if (GlobalParams.exploreSolvingGuesses){
							cout << endl << endl;
							BACKGROUND(CYAN_B)
							cout << space << "Let's take a look at what happens after guessing that ;)";
							BACKGROUND(DEFAULT);
							cout << endl;
							LinSolver* justALook(Lsolver->copy(hyp, filling));
							justALook->lineSolve(true, false);
							delete(justALook);
							cout << endl;
							if (GlobalParams.checkAmbiguity){
								BACKGROUND(CYAN_B)
								cout << space << "Let's get back to the seriousness of checking uniqueness ;)";
								BACKGROUND(DEFAULT);
								cout << " " << endl << endl;
							}
						}

						if (not GlobalParams.checkAmbiguity){
							BACKGROUND(CYAN_B)
							cout << space << "The solution is CONSIDERED unique and we do not check uniqueness";
							BACKGROUND(DEFAULT);
							cout << "." << endl;
							assert(nSol == 1);
							return 1;
						}
					}

					// Store both solver for later. However, their original pointer ceased to exist.
					if (filling == Full){
						hyp_full = hypSolver;
						out_full = hypOut;
					} else {
						hyp_empt = hypSolver;
						out_empt = hypOut;
					}
				}

				if (GlobalParams.printGuessStructure) cout << space << "(#, x) ==> ( (" << out_full.flag << ", " << out_full.modifCells << "), (" << out_empt.flag << ", " << out_empt.modifCells << ") )" << endl;
				if (out_full.flag != Stuck and out_empt.flag != Stuck){
					if (GlobalParams.printGuessStructure) cout << space << "DBG - No need to continue this branch" << endl;
					return nSol;
				}

				/* At this point, we know that at least one of them has been stuck. */

				if (out_full.flag == Stuck and out_empt.flag != Stuck){
					// Either (Stuck, Unique) or (Stuck, NoSol)...
					// Same thing since the empt-branch has been explored entirely.
					grid->notifyChange();
					fastForward = false;
					Lsolver->change(hyp.i, hyp.j, Full);
				}

				if (out_full.flag != Stuck and out_empt.flag == Stuck){
					// Either (Unique, Stuck) or (NoSol, Stuck)...
					// Same thing since the full-branch has been explored entirely.
					grid->notifyChange();
					fastForward = false;
					Lsolver->change(hyp.i, hyp.j, Empt);
				}


				if (out_full.flag == Stuck and out_empt.flag == Stuck){
					justStuckStuck = true;
					grid->set(new HypHolder({
						(out_full.modifCells > out_empt.modifCells ? out_full.modifCells : out_empt.modifCells),
						(out_full.modifCells > out_empt.modifCells ? out_empt.modifCells : out_full.modifCells),
						hyp.i,
						hyp.j,
						(out_full.modifCells > out_empt.modifCells ? Full : Empt)
					}));
				}

				delete(hyp_full);
				delete(hyp_empt);
			} else {
				if (GlobalParams.printRecursiveStructure) cout << space << "We need to go a level deeper." << endl;
				grid->print();

				HypHolder chosen(grid->getBest());

				ComplexSolver* best_choice(new ComplexSolver(this, chosen.i, chosen.j, chosen.best));
				int nSol_best(best_choice->complexSolve());
				delete(best_choice);
				if (GlobalParams.printRecursiveStructure) cout << space << "best_choice found :" << nSol_best << endl;

				ComplexSolver* sec_choice(new ComplexSolver(this, chosen.i, chosen.j, (chosen.best == Full ? Empt : Full)));
				int nSol_Sec(sec_choice->complexSolve());
				delete(sec_choice);

				if (GlobalParams.printRecursiveStructure) cout << space << "second_choice found :" << nSol_Sec << endl;
				return nSol + nSol_best + nSol_Sec;
			}
		} while (true);
		return 0;
	}
};

class Game{
private:
	ifstream input;
	size_t n;
	size_t m;
	vector<Blocks> BlocksInRow;
	vector<Blocks> BlocksInCol;
	ComplexSolver* Csolver = nullptr;
public:
	Game(string filename){
		input.open(filename);
		input >> n;
		input >> m;
		cout << "Opening a " << n << " x " << m << " Nonogram, needing " << n + m + 1 << " total lines." << endl;
		string line;
		getline(input, line);

		BlocksInRow.resize(n);
		BlocksInCol.resize(m);

		unsigned int totalRows(0);
		for (size_t i(0); i < n; i++){
			getline(input, line);
			totalRows += BlocksInRow[i].init(line);
		}
		unsigned int totalCols(0);
		for (size_t i(0); i < m; i++){
			getline(input, line);
			totalCols += BlocksInCol[i].init(line);
		}
		if (totalRows != totalCols){
OUTPUTCOLOR(RED_F)
			cout << "Error!" << endl;
OUTPUTCOLOR(DEFAULT)
			cout << "The number of cells in each directions do not match:" << endl;
			cout << totalRows << " according to rows and" << endl << totalCols << " according to columns." << endl;
			throw invalid_argument("Error in file. Please verify it has " + to_string(n+m+1) + " lines.");
		}
		Csolver = new ComplexSolver(n, m, BlocksInRow, BlocksInCol);
	}


	void manual_solve(){
		linSolvOu_t output({Stuck, 0});
		while (output.flag == Stuck){
			output = Csolver->Lsolver->lineSolve();
			switch (output.flag){

				case NoSol:
					cout << "The solver has deteted an error." << endl;
					break;

				case Stuck:
					cout << endl;
					cout << "The solver has ";
	OUTPUTCOLOR(RED_F)
					cout << "not managed"; OUTPUTCOLOR(DEFAULT)
					cout << " to solve the level. Can you make a guess and help ?" << endl;
					cout << "Enter x, y and the new value (#, x, \') to make a move: ";
					size_t x, y;
					char newValue_char;
					cin >> x;
					cin >> y;
					cin >> newValue_char;
					cell newV;

					switch (newValue_char){
						case '#':
							newV = Full;
							break;
						case 'x':
							newV = Empt;
							break;
						case '\'':
							newV = Void;
							break;
					}

					Csolver->Lsolver->change(x, y, newV);
					break;

				case Unique:
					cout << "Solution found!" << endl;
					Csolver->Lsolver->print(true);
					break;
			}
		}
	}

	void solve(){
		int n_solve (Csolver->complexSolve());
		cout << endl;
		cout << "The solver found ";
		OUTPUTCOLOR(CYAN_F)
		cout << n_solve;
		OUTPUTCOLOR(DEFAULT)
		cout << " solution";
		if (1 < n_solve) cout << "s";
		cout << " to the Nonogram." << endl;

		cout << endl;
		for (size_t i(0); i < GLOBAL_SOLUTIONS.size(); i++){
			GLOBAL_SOLUTIONS[i]->print(true);
		}
	}
};

bool test_checkLine(Blocks blocks, line_t lin, line_t goal){
	line_t out(lin.size(), Empt);

	cout << "[";
	for (auto const& val:lin) cout << val;
	cout << "]";
	cout << " ";
	for (auto const& val:blocks.v) cout << val << " ";
	cout << endl;

	bool succesFlag(checkLine(blocks, lin, out));

	// Got
	OUTPUTCOLOR(CYAN_F)
	cout << "> Got :" << endl;
	OUTPUTCOLOR(DEFAULT)
	if (succesFlag) {
		cout << "[";
		for (auto const& val:out) cout << val;
		cout << "]" << endl;
	} else {
		OUTPUTCOLOR(CYAN_F)
		cout << "An error!" << endl;
		OUTPUTCOLOR(DEFAULT)
	}

	// Expected
	OUTPUTCOLOR(CYAN_F)
	cout << "> Expected :" << endl;
	OUTPUTCOLOR(DEFAULT)
	if (goal.size() != 0) {
		cout << "[";
		for (auto const& val:goal) cout << val;
		cout << "]" << endl;
	} else {
		OUTPUTCOLOR(CYAN_F)
		cout << "An error!" << endl;
		OUTPUTCOLOR(DEFAULT)
	}

	if ((succesFlag and out != goal) or (not succesFlag and goal.size() != 0)){
		OUTPUTCOLOR(RED_F)
		cout << "> Test failed" << endl;
		OUTPUTCOLOR(DEFAULT)
		return 0;
	}
	OUTPUTCOLOR(GREEN_F)
	cout << "> Succes" << endl;
	OUTPUTCOLOR(DEFAULT)
	return 1;
}



int main(int argc, char *argv[]) {
	GLOBAL_SOLUTIONS.resize(0);

	setGlobalParameters();



	// Always run tests and print them only if wanted or errors


	string filename;
	if (argc == 2){
		filename = argv[1];
	} else {

		cout << "Starting test suit. Call the program directly with file in argument in order to skip tests." << endl;
		int succesCount(0);
		int testCount(1);

OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({2}),
										line_t({Void, Void, Void, Void, Void}),
										line_t({Void, Void, Void, Void, Void}));


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({5}),
										line_t({Void, Void, Void, Void, Void}),
										line_t({Full, Full, Full, Full, Full}));


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({3}),
										line_t({Void, Void, Void, Void, Void}),
										line_t({Void, Void, Full, Void, Void}));


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({3, 2, 1}),
										line_t({Void, Void, Void, Void, Void, Void, Void, Void, Void}),
										line_t({Void, Full, Full, Void, Void, Full, Void, Void, Void}));


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({3}),
										line_t({Void, Void, Void, Empt, Void}),
										line_t({Full, Full, Full, Empt, Empt}));


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({2}),
										line_t({Void, Void, Empt, Void, Empt, Void, Void}),
										line_t({Void, Void, Empt, Empt, Empt, Void, Void}));


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({2, 3, 1}),
										line_t({Void, Void, Void, Empt, Void, Empt, Void, Void, Void, Empt, Void, Void, Empt, Void}),
										line_t({Void, Full, Void, Empt, Empt, Empt, Full, Full, Full, Empt, Void, Void, Empt, Void}));



		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({1}),
										line_t({Full, Full}),
										line_t({}));


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({1, 2}),
										line_t({Full, Void, Void, Void, Void, Void, Void}),
										line_t({Full, Empt, Void, Void, Void, Void, Void}));


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({4, 2}),
										line_t({Void, Full, Void, Void, Void, Void, Void, Void, Full, Void, Void}),
										line_t({Void, Full, Full, Full, Void, Empt, Empt, Void, Full, Void, Empt}));



		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({5}),
										line_t({Full, Void, Void, Void, Void, Void, Void, Void, Void, Void, Void, Void}),
										line_t({Full, Full, Full, Full, Full, Empt, Empt, Empt, Empt, Empt, Empt, Empt}));


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({1, 3}),
										line_t({Void, Void, Void, Empt, Full, Void, Empt, Void, Void, Void, Void}),
										line_t({Empt, Empt, Empt, Empt, Full, Empt, Empt, Void, Full, Full, Void}));


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({1, 3, 1}),
										line_t({Void, Void, Void, Empt, Full, Void, Empt, Void, Void, Void, Void, Void, Void}),
										line_t({Empt, Empt, Empt, Empt, Full, Empt, Empt, Void, Full, Full, Void, Void, Void}));


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({3, 1, 2, 4}),
										line_t({Void, Void, Void, Void, Full, Void, Void, Void, Void, Void, Void, Void, Void, Full, Void}),
										line_t({Void, Void, Full, Void, Full, Empt, Void, Void, Void, Void, Void, Full, Full, Full, Void}));


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({3, 1, 2, 4}),
										line_t({Void, Void, Void, Void, Full, Void, Void, Void, Void, Void, Void, Void, Void, Full, Void}),
										line_t({Void, Void, Full, Void, Full, Empt, Void, Void, Void, Void, Void, Full, Full, Full, Void}));


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({1, 2, 1, 2, 1}),
										line_t({Void, Void, Void, Void, Full, Void, Full, Full, Void, Full, Void, Void, Void, Void, Void}),
										line_t({Empt, Empt, Empt, Empt, Full, Empt, Full, Full, Empt, Full, Empt, Full, Full, Empt, Full}));


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({2, 2, 2}),
										line_t({Void, Void, Void, Void, Void, Void, Empt, Full, Void, Void, Void, Void, Void, Void}),
										line_t({Void, Void, Void, Void, Void, Void, Empt, Full, Full, Empt, Void, Void, Void, Void}));

		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({2, 1}),
										line_t({Void, Void, Empt, Full, Void, Void, Void}),
										line_t({Void, Void, Empt, Full, Void, Empt, Void}));


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({2, 5, 2}),
										line_t({Void, Void, Void, Void, Void, Void, Void, Full, Full, Void, Void, Full, Empt, Void, Void}),
										line_t({Void, Void, Void, Void, Void, Void, Void, Full, Full, Void, Full, Full, Empt, Void, Void}));


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({2, 1, 3, 1}),
										line_t({Empt, Void, Full, Void, Void, Void, Full, Void, Void, Full, Void, Void, Void, Void, Void}),
										line_t({Empt, Void, Full, Void, Empt, Empt, Full, Empt, Void, Full, Full, Void, Void, Void, Void}));


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({3, 1, 1, 3, 1}),
										line_t({Void, Void, Void, Void, Full, Void, Void, Full, Void, Void, Full, Void, Void, Void, Void, Void, Void, Void, Void, Void}),
										line_t({Void, Void, Void, Void, Full, Void, Empt, Full, Empt, Void, Full, Void, Void, Void, Void, Void, Void, Void, Void, Void}));


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({4, 1, 5, 2}),
										line_t({Void, Void, Void, Void, Void, Void, Void, Void, Void, Full, Void, Void, Full, Full, Void, Void, Void, Void, Void, Full, Void, Void, Void, Void, Void}),
										line_t({Void, Void, Void, Void, Void, Void, Void, Void, Empt, Full, Void, Void, Full, Full, Void, Void, Void, Empt, Void, Full, Void, Empt, Empt, Empt, Empt}));



		cout << endl;
		if (succesCount < testCount) {
			cout << "Some test failed: " << succesCount << "/" << testCount << endl;
			return 0;
		}
		cout << "ALL TEST SUCESSFULL: " << succesCount << "/" << testCount << endl << endl;
		cout << "Please enter the file name: " << endl;
		cin >> filename;
	}

	explainColors();

	Game game(filename);


	//game.manual_solve();
	game.solve();



	cout << endl << "exiting normally..." << endl;
    return 0;
}
