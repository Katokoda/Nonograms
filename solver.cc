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

#define ALTERNATE_BACKGROUND true
#define ALTERNATE_EVERYWHERE true
#define PRINT_EVERY_CHANGE false
#define PAUSE_EVERY_SEMIITER true
#define FAST_FORWARD_DEFAULT false
#define PRINT_DETAIL_LINES false

#define CHECK_AMBIGUITY true

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
				cout << "\033[37m";
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
	cout << "The color and value of the character will depend on the nummber according to the list below." << endl;
	cout << "(put your number on the list such that each written digit matches," << endl;
	cout << " the 'x' will be displayed in the matching color)" << endl;

	cout << "\033[37m" << "00x" << "  ";
	cout << "\033[33m" << "01x" << "  ";
	cout << "\033[32m" << "02x" << "  ";
	cout << "\033[36m" << "03x" << "  ";
	cout << "\033[96m" << "04x" << "  ";
	cout << "\033[34m" << "05x" << "  ";
	cout << "\033[94m" << "06x" << "  ";
	cout << "\033[35m" << "07x" << "  ";
	cout << "\033[95m" << "08x" << "  ";
	cout << "\033[90m" << "09x" << endl;
	cout << "\033[93m" << "x--" << "  ";
	cout << "\033[31m" << "(every number bigger than 999 will be a red *)" << endl;
	OUTPUTCOLOR(DEFAULT)
	cout << endl << "Numbers {00, 11, 22, ..., 99, 110, 121}" << endl;
	for (int n(0); n < 130; n+=11){
		printOneNumber(n);
	}
	cout << endl << "Numbers {64, 128, 256, 512, 1024}" << endl;
	for (int n(64); n < 2000; n*=2){
		printOneNumber(n);
	}
	
	cout << endl << endl;
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

//TODO LIST
/*
Implémenter un vérificateur sur la somme de cases pleines (chaque côté)
	- Si la somme est différente, print le nombre de lignes lues et le nombre de ligne totales que le fichier doit avoir
Lire les arguments de la ligne de commande à la place de MACROS
*/



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

bool leftMostConfiguration(Blocks const& blocks, line_t const& lin, vector<size_t>& res){
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
OUTPUTCOLOR(RED_F)
			cout << "Error!" << endl;
OUTPUTCOLOR(DEFAULT)
			cout << " Can't cover with any blocks" << endl;
				return false;
			}
			//cout << "with block  " << idx << " at " << res[idx] << endl;
			if (blocks.v[idx] < givenBlocks[givB_Idx].rightMost - givenBlocks[givB_Idx].leftMost + 1){
				//cout << "can't cover" << endl; // too short
				res[idx] = givenBlocks[givB_Idx].rightMost + 1;
			} else {
				if (givenBlocks[givB_Idx].rightMost <= res[idx] + blocks.v[idx] - 1){
					//cout << "covered already" << endl;
				} else {
					res[idx] = givenBlocks[givB_Idx].rightMost + 1 - blocks.v[idx];
					//cout << "w-------->  " << idx << " at " << res[idx] << endl;
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
OUTPUTCOLOR(RED_F)
				cout << "ERROR"; OUTPUTCOLOR(DEFAULT)
				cout << " - resetScanning before Emptiness" << endl;
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
OUTPUTCOLOR(RED_F)
						cout << "ERROR"; OUTPUTCOLOR(DEFAULT)
						cout << " - resetScanning after Emptiness" << endl;
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
OUTPUTCOLOR(RED_F)
						cout << "ERROR"; OUTPUTCOLOR(DEFAULT)
						cout << " - after getting pulled - potStart == n" << endl;
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
OUTPUTCOLOR(RED_F)
				cout << "ERROR"; OUTPUTCOLOR(DEFAULT)
				cout << " - during sliding - (not enough space)" << endl;
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

bool rightMostConfiguration(Blocks const& blocks, line_t const& lin, vector<size_t>& res){
	size_t n(lin.size());
	line_t lin_inv(n, Void);
	for (size_t i(0); i < n; i++) lin_inv[n - i - 1] = lin[i];

	Blocks blocks_inv(blocks.v, true);

	OUTPUTCOLOR(YELLOW_F)
	vector<size_t> inverted(blocks.v.size(), 0);
	bool flag(leftMostConfiguration(blocks_inv, lin_inv, inverted));
	if (not flag) return false;
	OUTPUTCOLOR(DEFAULT)
	size_t k(inverted.size());

	for (size_t i(0); i < k; i++) res[k - i - 1] = n - inverted[i] - blocks_inv.v[i];

	return true;
}


bool checkLine(Blocks const& blocks, line_t const& lin, line_t& res, bool verb = false){
	size_t n(lin.size());
	size_t k(blocks.v.size());

	for (size_t i(0); i < n; i++){
		if (lin[i] != Void) res[i] = lin[i];
	}

#ifdef DEBUG_CHECKLINE
		cout << "Calling checkLine on" << endl;
		cout << "\"";
		for (auto const& val:lin) cout << val;
		cout << "\" with blocks [";
		for (auto const& val:blocks.v) cout << val << " ";
		cout << "] gives :" << endl;
#endif

	// Find LEFT-most configuration
	vector<size_t> leftMost(blocks.v.size(), 0); // leftMost[i] = left-most square of the i-th block in the left-most configuration
	bool flag ( leftMostConfiguration(blocks, lin, leftMost));
	if (not flag) return false;

	// Find RIGHT-most configuration
	vector<size_t> rightMost(blocks.v.size(), 0); // rightMost[i] = right-most square of the i-th block in the right-most configuration
	flag = (rightMostConfiguration(blocks, lin, rightMost));
	if (not flag) return false;

#ifdef SCANNING_DEBUG
	cout << endl << endl;
	cout << "Continuing checkLine on" << endl << "\"";
	for (auto const& val:lin) cout << val;
	cout << "\" with blocks [";
	for (auto const& val:blocks.v) cout << val << " ";
	cout << "]." << endl;
	cout << endl;
#endif


	for (size_t idx(0); idx < k; idx++){
#ifdef DEBUG_CHECKLINE
		if (verb) cout << "DBG - checkLine - block " << idx << " of length " << blocks.v[idx] << endl;
		if (verb) cout << "[" << leftMost[idx] << ", " << rightMost[idx] << "]" << endl;
#endif


		// draw intersection of both extremes if exists
		for (size_t pos(rightMost[idx]); pos < leftMost[idx] + blocks.v[idx]; pos++) res[pos] = Full;


		// slide block from left-most to right-most and mark only possible slots
		size_t potStart(leftMost[idx]);
		size_t scaner(leftMost[idx]);
		while (potStart <= rightMost[idx]){
			#ifdef SCANNING_DEBUG
			cout << "Maybying - Start" << endl;
			printScanning(potStart, scaner, res);
			#endif

			while (scaner < potStart + blocks.v[idx] and lin[scaner] != Empt){
				scaner++;
			}

			#ifdef SCANNING_DEBUG
			cout << "Maybying - foundEnd" << endl;
			printScanning(potStart, scaner, res);
			#endif

			if (scaner == potStart + blocks.v[idx]){
				if ((scaner == n or lin[scaner] != Full) and (potStart == 0 or lin[potStart-1] != Full)){
					for (size_t pos(potStart); pos < scaner; pos++){
						if (res[pos] != Full) res[pos] = Void;
					}
				}
				potStart++;
			} else if (scaner < potStart + blocks.v[idx]){
				while (lin[scaner] == Empt) scaner++;
				potStart = scaner;
			}

			#ifdef SCANNING_DEBUG
			cout << "Maybying - End" << endl;
			printScanning(potStart, scaner, res, true);
			#endif
		}
	}
	if (verb){
		cout << "\"";
		for (auto const& val:res) cout << val;
		cout << "\"" << endl << endl;
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

	tab_t tab;
	vector<size_t> countInRow;
	vector<bool> modifRow;
	vector<bool> modifCol;
public:
	State(size_t n_, size_t m_, vector<Blocks> const& BlocksInRow_, vector<Blocks> const& BlocksInCol_)
	:n(n_), m(m_), count(0), BlocksInRow(BlocksInRow_), BlocksInCol(BlocksInCol_)
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
		State* other(new State(n, m, BlocksInRow, BlocksInCol));
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
			modifCol[j] = true;
			countInRow[i]++;
			count++;
		} else {
			cout << "ERROR - playing at non-void" << endl;
		}
	}

	size_t getCount() const{
		return count;
	}

	bool isSolved() const{
		return count == n*m;
	}

	void print(bool pretty = false) const{
		cout << "┌";
		for (size_t i(0); i<m; i++) cout << "─";
		cout << "┐" << endl;

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
					if (ALTERNATE_BACKGROUND and (i+j)%2 == 0 and (ALTERNATE_EVERYWHERE or tab[i][j] == Void)) cout << "\e[40m";
					cout << tab[i][j];
					BACKGROUND(DEFAULT);
				}
			}
			if (not modifRow[i]) OUTPUTCOLOR(GREEN_F);
			cout << "│";
			OUTPUTCOLOR(DEFAULT);
			for (auto const& val: BlocksInRow[i].v) cout << " " << val;
			cout << endl;
		}

		cout << "└";
		for (size_t i(0); i<m; i++){
			if (not modifCol[i]) OUTPUTCOLOR(GREEN_F);
			cout << "─";
			OUTPUTCOLOR(DEFAULT);
		}
		cout << "┘" << endl;

		bool stillColPrinting(true);
		for (size_t j(0); stillColPrinting; j++){
			stillColPrinting = false;
			cout << " ";
			for (size_t i(0); i<m; i++){
				if (j < BlocksInCol[i].v.size()){
					if (j +1 < BlocksInCol[i].v.size()) stillColPrinting = true;
					if (BlocksInCol[i].v[j] < 10){
						cout << BlocksInCol[i].v[j];
					} else {
						OUTPUTCOLOR(CYAN_F)
						cout << (BlocksInCol[i].v[j] % 10);
						OUTPUTCOLOR(DEFAULT)
					}
				} else {
					cout << " ";
				}
			}
			cout << " " << endl;
		}

		cout << endl;
	}

	int checkRow(Blocks const& blocks, size_t i){
		// return {1} if modified something {0} if modified nothing and {-1} for errors.

		 // if modifRow[i] == False, we do nothing.
		if (not modifRow[i]) return 0;
		modifRow[i] = false; // Otherwise, we set it to false for detecting changes.

		if (PRINT_DETAIL_LINES) cout << "Check row " << i << endl;

		line_t lin;
		lin.resize(m);
		for (size_t k(0); k < m; k++) lin[k] = tab[i][k];

		line_t out(m, Empt);
		bool succesFlag(checkLine(blocks, lin, out, PRINT_DETAIL_LINES));
		if (not succesFlag){
			if (PRINT_DETAIL_LINES) {
				OUTPUTCOLOR(RED_F);
				cout << "ERROR";
				OUTPUTCOLOR(DEFAULT);
				cout << " - something is wrong in that line" << endl;
			}
			return -1;
		}
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
		if (PRINT_DETAIL_LINES) cout << "Check col " << i << endl;
		modifCol[i] = false;

		line_t lin;
		lin.resize(n);
		for (size_t k(0); k < n; k++) lin[k] = tab[k][i];

		line_t out(n, Empt);
		bool succesFlag(checkLine(blocks, lin, out, PRINT_DETAIL_LINES));
		if (not succesFlag){
			if (PRINT_DETAIL_LINES) {
				OUTPUTCOLOR(RED_F);
				cout << "ERROR";
				OUTPUTCOLOR(DEFAULT);
				cout << " - something is wrong in that line" << endl;
			}
			return -1;
		}

		for (size_t k(0); k < n; k++){
			if (out[k] != Void and out[k] != lin[k]) play(k, i, out[k]);
		}

		bool flag(modifCol[i]); // if modifRow has been set to true (by a modification), then we return true.
		modifCol[i] = false;
		if (flag) return 1;
		return 0;
	}
};

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

	void print(bool pretty = false){
		state->print(pretty);
	}

	linSolvOu_t lineSolve(bool verbose = false){
		size_t filledBefore(state->getCount());
		bool changed(true);
		bool fastForward(false);
		while (changed){

			if (verbose and not fastForward){
				print();
				cout << "Press Enter to check rows" << endl;
				fastForward = checkEntryAndFF();
			}

			changed = false;
			for (size_t i(0); i < n; i++){
				int flag(state->checkRow(BlocksInRow[i], i));
				if (flag < 0) return {NoSol, (unsigned int)(state->getCount() - filledBefore)};
				changed = (flag or changed);
			}

			changed = false;
			if (verbose and not fastForward){
				print();
				cout << "Press Enter to check columns" << endl;
				fastForward = checkEntryAndFF();
			}

			for (size_t i(0); i < m; i++){
				int flag(state->checkCol(BlocksInCol[i], i));
				if (flag < 0) return {NoSol, (unsigned int)(state->getCount() - filledBefore)};
				changed = (flag or changed);
			}
		}
		if (verbose) print();
		if (state->isSolved()){
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
		cout << "┌";
		for (size_t i(0); i<m; i++) cout << "─";
		cout << "┐" << endl;

		for (size_t i(0); i < n; i++){
			cout << "│";
			for (size_t j(0); j < m; j++){
				if (lastHyp.i == i and lastHyp.j == j) BACKGROUND(GREEN_B)
				if (grid[i][j] != nullptr){
					printOneNumber(grid[i][j]->n_main);
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
		cout << "WARNING - TODO - implement sorting" << endl;
		return *grid[0][0];
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


	unsigned int complexSolve(bool unsureAboutUniqueness = CHECK_AMBIGUITY, bool wantToSeeDetails = true){
		unsigned int nSol(0);
		bool fastForward(false);
		bool justStuckStuck(false);
		do {
			linSolvOu_t out(Lsolver->lineSolve((depth == 0) and wantToSeeDetails and not justStuckStuck and not fastForward));
			if (0 < out.modifCells){
				fastForward = false;
				grid->notifyChange();
			}
			justStuckStuck = false;

			switch (out.flag){
				case Unique:
					cout << space << "A unique solution has been found:" << endl;
					Lsolver->print();
					return nSol+1;
				case NoSol:
					cout << space << "No valid solution - impossible problem" << endl;
					return nSol+0;
				default:
					cout << endl;
					cout << space << "The simple lin-solver is ";
					BACKGROUND(RED_B)
					cout << "stuck";
					BACKGROUND(DEFAULT)
					cout << ":" << endl;
					Lsolver->print();
			}

			gridPTR hyp({n, m});
			bool validHypFlag(grid->get(Lsolver->state, hyp));
			grid->print();

			if (validHypFlag){
				cout << space << "hyp on (" << hyp.i << ", " << hyp.j << ")" << endl;
				if (wantToSeeDetails and not fastForward){
					cout << space << "Press Enter to continue." << endl;
					fastForward = checkEntryAndFF();
				}

				LinSolver* hyp_full(Lsolver->copy(hyp, Full));
				linSolvOu_t out_full(hyp_full->lineSolve(false));
				if (0 < out_full.modifCells){
					if (wantToSeeDetails and out_full.flag == Unique) {
						cout << "The solver just found out that the guess # on (" << hyp.i << ", " << hyp.j << ") gives a solution." << endl;
					} else {
						hyp_full->print();
					}
				}
				cout << space << "# --> (" << out_full.flag << ", " << out_full.modifCells << ")" << endl;
				if (out_full.flag == Unique){
					nSol++;
					cout << space << "TODO - Store solution" << endl;
					if (wantToSeeDetails){
						cout << endl << endl;
						BACKGROUND(CYAN_B)
						cout << space << "Let's take a look at what happens after guessing that ;)";
						BACKGROUND(DEFAULT);
						cout << endl;
						LinSolver* justALook(Lsolver->copy(hyp, Full));
						justALook->lineSolve(true);
						delete(justALook);
						cout << endl;
						if (unsureAboutUniqueness){
							BACKGROUND(CYAN_B)
							cout << space << "Let's get back to the seriousness of checking uniqueness ;)";
							BACKGROUND(DEFAULT);
							cout << " " << endl << endl;
						}
					}
					if (not unsureAboutUniqueness){
						if (unsureAboutUniqueness){
							BACKGROUND(CYAN_B)
						}
						cout << space << "The solution is CONSIDERED unique and we do not check uniqueness";
						if (unsureAboutUniqueness){
							BACKGROUND(DEFAULT);
						}
						cout << "." << endl;
						assert(nSol == 1);
						return 1;
					}
				}

				LinSolver* hyp_empt(Lsolver->copy(hyp, Empt));
				linSolvOu_t out_empt(hyp_empt->lineSolve(false));
				if (0 < out_empt.modifCells){
					if (wantToSeeDetails and out_empt.flag == Unique) {
						cout << "The solver just found out that the guess x on (" << hyp.i << ", " << hyp.j << ") gives a solution.";
					} else {
						hyp_empt->print();
					}
				}
				cout << space << "x --> (" << out_empt.flag << ", " << out_empt.modifCells << ")" << endl;
				if (out_empt.flag == Unique){
					nSol++;
					cout << space << "TODO - Store solution" << endl;
					if (wantToSeeDetails){
						cout << endl << endl;
						BACKGROUND(CYAN_B)
						cout << space << "Let's take a look at what happens after guessing that ;)";
						BACKGROUND(DEFAULT);
						cout << endl;
						LinSolver* justALook(Lsolver->copy(hyp, Empt));
						justALook->lineSolve(true);
						delete(justALook);
						if (unsureAboutUniqueness){
							BACKGROUND(CYAN_B)
							cout << space << "Let's get back to the seriousness of checking uniqueness ;)";
							BACKGROUND(DEFAULT);
							cout << " " << endl << endl;
						}
					}
					if (not unsureAboutUniqueness){
						if (unsureAboutUniqueness){
							BACKGROUND(CYAN_B)
						}
						cout << space << "The solution is CONSIDERED unique and we do not check uniqueness";
						if (unsureAboutUniqueness){
							BACKGROUND(DEFAULT);
						}
						cout << "." << endl;
						assert(nSol == 1);
						return 1;
					}
				}


				cout << space << "(#, x) ==> (" << out_full.flag << ", " << out_empt.flag << ")" << endl;
				if (out_full.flag != Stuck and out_empt.flag != Stuck){
					cout << space << "DBG - No need to continue this branch" << endl;
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
				cout << space << "We need to go a level deeper." << endl;
				grid->print();

				HypHolder chosen(grid->getBest());

				ComplexSolver* best_choice(new ComplexSolver(this, chosen.i, chosen.j, chosen.best));
				int nSol_best(best_choice->complexSolve());
				delete(best_choice);
				cout << space << "best_choice found :" << nSol_best << endl;

				ComplexSolver* sec_choice(new ComplexSolver(this, chosen.i, chosen.j, (chosen.best == Full ? Empt : Full)));
				int nSol_Sec(sec_choice->complexSolve());
				delete(sec_choice);

				cout << space << "second_choice found :" << nSol_Sec << endl;
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
		cout << " solution(s) to the Nonogram." << endl;
	}
};

bool test_checkLine(Blocks blocks, line_t lin, line_t goal, bool verb = false){
	line_t out(lin.size(), Empt);
	bool succesFlag(checkLine(blocks, lin, out, verb));

	// Got
	OUTPUTCOLOR(CYAN_F)
	cout << "> Got :" << endl;
	OUTPUTCOLOR(DEFAULT)
	if (succesFlag) {
		cout << "\"";
		for (auto const& val:out) cout << val;
		cout << "\"" << endl;
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
		cout << "\"";
		for (auto const& val:goal) cout << val;
		cout << "\"" << endl;
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
										line_t({Void, Void, Void, Void, Void}), true);


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({5}),
										line_t({Void, Void, Void, Void, Void}),
										line_t({Full, Full, Full, Full, Full}), true);


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({3}),
										line_t({Void, Void, Void, Void, Void}),
										line_t({Void, Void, Full, Void, Void}), true);


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({3, 2, 1}),
										line_t({Void, Void, Void, Void, Void, Void, Void, Void, Void}),
										line_t({Void, Full, Full, Void, Void, Full, Void, Void, Void}), true);


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({3}),
										line_t({Void, Void, Void, Empt, Void}),
										line_t({Full, Full, Full, Empt, Empt}), true);


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({2}),
										line_t({Void, Void, Empt, Void, Empt, Void, Void}),
										line_t({Void, Void, Empt, Empt, Empt, Void, Void}), true);


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({2, 3, 1}),
										line_t({Void, Void, Void, Empt, Void, Empt, Void, Void, Void, Empt, Void, Void, Empt, Void}),
										line_t({Void, Full, Void, Empt, Empt, Empt, Full, Full, Full, Empt, Void, Void, Empt, Void}), true);



		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({1}),
										line_t({Full, Full}),
										line_t({}), true);


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({1, 2}),
										line_t({Full, Void, Void, Void, Void, Void, Void}),
										line_t({Full, Empt, Void, Void, Void, Void, Void}), true);


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({4, 2}),
										line_t({Void, Full, Void, Void, Void, Void, Void, Void, Full, Void, Void}),
										line_t({Void, Full, Full, Full, Void, Empt, Empt, Void, Full, Void, Empt}), true);



		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({5}),
										line_t({Full, Void, Void, Void, Void, Void, Void, Void, Void, Void, Void, Void}),
										line_t({Full, Full, Full, Full, Full, Empt, Empt, Empt, Empt, Empt, Empt, Empt}), true);


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({1, 3}),
										line_t({Void, Void, Void, Empt, Full, Void, Empt, Void, Void, Void, Void}),
										line_t({Empt, Empt, Empt, Empt, Full, Empt, Empt, Void, Full, Full, Void}), true);


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({1, 3, 1}),
										line_t({Void, Void, Void, Empt, Full, Void, Empt, Void, Void, Void, Void, Void, Void}),
										line_t({Empt, Empt, Empt, Empt, Full, Empt, Empt, Void, Full, Full, Void, Void, Void}), true);


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({3, 1, 2, 4}),
										line_t({Void, Void, Void, Void, Full, Void, Void, Void, Void, Void, Void, Void, Void, Full, Void}),
										line_t({Void, Void, Full, Void, Full, Empt, Void, Void, Void, Void, Void, Full, Full, Full, Void}), true);


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({3, 1, 2, 4}),
										line_t({Void, Void, Void, Void, Full, Void, Void, Void, Void, Void, Void, Void, Void, Full, Void}),
										line_t({Void, Void, Full, Void, Full, Empt, Void, Void, Void, Void, Void, Full, Full, Full, Void}), true);


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({1, 2, 1, 2, 1}),
										line_t({Void, Void, Void, Void, Full, Void, Full, Full, Void, Full, Void, Void, Void, Void, Void}),
										line_t({Empt, Empt, Empt, Empt, Full, Empt, Full, Full, Empt, Full, Empt, Full, Full, Empt, Full}), true);


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({2, 2, 2}),
										line_t({Void, Void, Void, Void, Void, Void, Empt, Full, Void, Void, Void, Void, Void, Void}),
										line_t({Void, Void, Void, Void, Void, Void, Empt, Full, Full, Empt, Void, Void, Void, Void}), true);

		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({2, 1}),
										line_t({Void, Void, Empt, Full, Void, Void, Void}),
										line_t({Void, Void, Empt, Full, Void, Empt, Void}), true);


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({2, 5, 2}),
										line_t({Void, Void, Void, Void, Void, Void, Void, Full, Full, Void, Void, Full, Empt, Void, Void}),
										line_t({Void, Void, Void, Void, Void, Void, Void, Full, Full, Void, Full, Full, Empt, Void, Void}), true);


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({2, 1, 3, 1}),
										line_t({Empt, Void, Full, Void, Void, Void, Full, Void, Void, Full, Void, Void, Void, Void, Void}),
										line_t({Empt, Void, Full, Void, Empt, Empt, Full, Empt, Void, Full, Full, Void, Void, Void, Void}), true);


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({3, 1, 1, 3, 1}),
										line_t({Void, Void, Void, Void, Full, Void, Void, Full, Void, Void, Full, Void, Void, Void, Void, Void, Void, Void, Void, Void}),
										line_t({Void, Void, Void, Void, Full, Void, Empt, Full, Empt, Void, Full, Void, Void, Void, Void, Void, Void, Void, Void, Void}), true);


		if (succesCount < testCount and STOPTEST) return 0;
		testCount++;
OUTPUTCOLOR(CYAN_F)
		cout << endl << "========= Test " << testCount << endl; OUTPUTCOLOR(DEFAULT)
		succesCount += test_checkLine(Blocks({4, 1, 5, 2}),
										line_t({Void, Void, Void, Void, Void, Void, Void, Void, Void, Full, Void, Void, Full, Full, Void, Void, Void, Void, Void, Full, Void, Void, Void, Void, Void}),
										line_t({Void, Void, Void, Void, Void, Void, Void, Void, Empt, Full, Void, Void, Full, Full, Void, Void, Void, Empt, Void, Full, Void, Empt, Empt, Empt, Empt}), true);




		if (succesCount < testCount) {
			cout << "Some test failed: " << succesCount << "/" << testCount << endl;
			return 0;
		}
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
