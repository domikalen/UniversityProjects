#ifndef __PROGTEST__
#include <cstring>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <memory>
#include <functional>
#include <stdexcept>
#endif /* __PROGTEST__ */
//---------------------------------------------------------------------------------------------
using namespace std;

class CEmpty;
class CText;
class CImage;
class CTable;
//---------------------------------------------------------------------------------------------

/**
 * Abstract base class representing a generic cell in a table.
 * Provides interface for cloning, dimension queries, rendering, and equality.
 */
class CCell
{
public:
    //---------------------------------------------------------------------------------------------
    /**
     * Virtual destructor for proper cleanup of derived cells.
     */
    virtual ~CCell() = default;

    /**
     * Create a deep copy of this cell.
     * @return pointer to newly cloned cell
     */
    virtual CCell* clone() const = 0;

    /**
     * Get the natural width of the cell content.
     * @return width in characters
     */
    virtual size_t getWidth() const = 0;

    /**
     * Get the natural height of the cell content.
     * @return number of text/image rows
     */
    virtual size_t getHeight() const = 0;

    /**
     * Render the cell into a block of text of given size.
     * @param width target width
     * @param height target height
     * @return vector of strings representing each line
     */
    virtual vector<string> printToOut(size_t width, size_t height) const = 0;

    /**
     * Compare this cell for equality with another.
     * @param b another cell
     * @return true if equal
     */
    virtual bool operator==(const CCell & b) const = 0;

    /**
     * Inequality operator.
     */
    bool operator!=(const CCell & b) const { return !(*this == b); }

    // Type-specific equality dispatch:
    virtual bool equals(const CEmpty & other) const = 0;
    virtual bool equals(const CText  & other) const = 0;
    virtual bool equals(const CImage & other) const = 0;
    virtual bool equals(const CTable & other) const = 0;
};
//---------------------------------------------------------------------------------------------

/**
 * Represents an empty cell, rendering as spaces.
 */
class CEmpty : public CCell
{
public:
    //---------------------------------------------------------------------------------------------
    /** Default constructor. */
    CEmpty() = default;

    /** Clone this empty cell.
     * @return new CEmpty instance
     */
    CCell* clone() const override { return new CEmpty(); }

    /** Always zero width. */
    size_t getWidth()  const override { return 0; }

    /** Always zero height. */
    size_t getHeight() const override { return 0; }

    /**
     * Render empty area filled with spaces.
     * @param width target width
     * @param height target height
     * @return lines of spaces
     */
    vector<string> printToOut(size_t width, size_t height) const override
    {
        vector<string> out;
        out.reserve(height);
        for (size_t i = 0; i < height; ++i)
            out.emplace_back(width, ' ');
        return out;
    }

    /**
     * Equality check: delegate to double dispatch.
     */
    bool operator==(const CCell & b) const override
    {
        return b.equals(*this);
    }

    bool equals(const CEmpty & other) const override { return true; }
    bool equals(const CText  & other) const override { return false; }
    bool equals(const CImage & other) const override { return false; }
    bool equals(const CTable & other) const override { return false; }
    //---------------------------------------------------------------------------------------------
};
//---------------------------------------------------------------------------------------------

/**
 * Text cell: holds multiline text and supports left/right alignment.
 */
class CText : public CCell
{
public:
    //---------------------------------------------------------------------------------------------
    static const int ALIGN_LEFT  = 0;  // Left-align text
    static const int ALIGN_RIGHT = 1;  // Right-align text

    /**
     * Construct text cell.
     * @param text input string
     * @param align alignment mode (ALIGN_LEFT or ALIGN_RIGHT)
     */
    CText(const string & text, int align) : align(align) { setText(text); }

    /** Clone this text cell. */
    CCell* clone() const override { return new CText(*this); }

    /**
     * Split input text by newline into rows.
     * @param text raw text with optional '\n'
     */
    void setText(const string & text)
    {
        rows.clear();
        size_t pos = 0, end;
        while (pos <= text.size())
        {
            end = text.find('\n', pos);
            if (end == string::npos) end = text.size();
            rows.push_back(text.substr(pos, end - pos));
            pos = end + 1;
        }
    }

    /**
     * Compute max line length.
     * @return width of longest row
     */
    size_t getWidth() const override
    {
        size_t w = 0;
        for (const auto & line : rows)
            w = max(w, line.size());
        return w;
    }

    /**
     * Number of rows.
     * @return row count
     */
    size_t getHeight() const override
    {
        return rows.size();
    }

    /**
     * Render text into fixed-size block with alignment.
     * @param width target width
     * @param height target height
     * @return aligned lines, padded with spaces
     */
    vector<string> printToOut(size_t width, size_t height) const override
    {
        vector<string> out;
        out.reserve(height);
        for (size_t r = 0; r < height; ++r)
        {
            string line = (r < rows.size()) ? rows[r] : string();
            size_t len = line.size();
            if (align == ALIGN_LEFT)            line += string(width - len, ' ');
            else /*ALIGN_RIGHT*/                line = string(width - len, ' ') + line;
            out.push_back(std::move(line));
        }
        return out;
    }

    /** Dispatch equality. */
    bool operator==(const CCell & b) const override
    {
        return b.equals(*this);
    }

    bool equals(const CEmpty & other) const override { return false; }
    bool equals(const CText  & other) const override{ return align == other.align && rows  == other.rows; }
    bool equals(const CImage & other) const override { return false; }
    bool equals(const CTable & other) const override { return false; }
    //---------------------------------------------------------------------------------------------
private:
    int align; // Alignment mode
    vector<string> rows; // Text rows
};
//---------------------------------------------------------------------------------------------

/**
 * Image cell: holds ASCII art rows and centers it in cell.
 */
class CImage : public CCell
{
public:
    //---------------------------------------------------------------------------------------------
    /** Default constructor. */
    CImage() = default;

    /**
     * Append a row of image ASCII art.
     * @param row text row
     * @return reference for chaining
     */
    CImage& addRow(const string & row)
    {
        rows.push_back(row);
        return *this;
    }

    /** Clone this image cell. */
    CCell* clone() const override { return new CImage(*this); }

    /**
     * Natural width = length of first row.
     */
    size_t getWidth()  const override { return rows.empty() ? 0 : rows[0].size(); }

    /**
     * Natural height = number of rows.
     */
    size_t getHeight() const override { return rows.size(); }

    /**
     * Render image centered within given size.
     * @param width target width
     * @param height target height
     * @return block of text with image centered
     */
    vector<string> printToOut(size_t width, size_t height) const override
    {
        vector<string> out(height, string(width, ' '));
        size_t iw = getWidth(), ih = getHeight();
        if (!iw || !ih) return out;
        size_t dx = (width  > iw ? (width  - iw) / 2 : 0);
        size_t dy = (height > ih ? (height - ih) / 2 : 0);
        for (size_t r = 0; r < ih && dy + r < height; ++r)
            for (size_t c = 0; c < iw && dx + c < width; ++c)
                out[dy + r][dx + c] = rows[r][c];
        return out;
    }

    /** Dispatch equality. */
    bool operator==(const CCell & b) const override{ return b.equals(*this); }

    bool equals(const CEmpty & other) const override { return false; }
    bool equals(const CText  & other) const override { return false; }
    bool equals(const CImage & other) const override { return rows == other.rows; }
    bool equals(const CTable & other) const override { return false; }
    //---------------------------------------------------------------------------------------------
private:
    vector<string> rows;      // ASCII art rows
};
//---------------------------------------------------------------------------------------------

/**
 * Table cell: composed of a grid of sub-cells, draws borders.
 */
class CTable : public CCell
{
public:
    //---------------------------------------------------------------------------------------------
    /**
     * Construct table with given rows and columns, initially empty.
     * @param R number of rows
     * @param C number of columns
     */
    CTable(size_t R, size_t C) : rows(R), cols(C)
    {
        cells.resize(rows);
        for (size_t r = 0; r < rows; ++r)
        {
            cells[r].reserve(cols);
            for (size_t c = 0; c < cols; ++c)
                cells[r].emplace_back(make_unique<CEmpty>());
        }
    }

    /** Copy constructor performs deep clone of all cells. */
    CTable(const CTable & o) : rows(o.rows), cols(o.cols), cells(rows)
    {
        for (size_t r = 0; r < rows; ++r)
            for (size_t c = 0; c < cols; ++c)
                cells[r].push_back(unique_ptr<CCell>(o.cells[r][c]->clone()));
    }

    /**
     * Copy-assignment using copy-and-swap idiom.
     */
    CTable& operator=(CTable o)
    {
        swap(*this, o);
        return *this;
    }

    /** Swap two tables (exchange all data). */
    friend void swap(CTable & a, CTable & b)
    {
        using std::swap;
        swap(a.rows,  b.rows);
        swap(a.cols,  b.cols);
        swap(a.cells, b.cells);
    }

    /** Clone this table.
     * @return deep-copied CTable
     */
    CCell* clone() const override { return new CTable(*this); }

    /**
     * Access non-const cell reference.
     */
    CCell& getCell(size_t r, size_t c)       { return *cells.at(r).at(c); }
    const CCell& getCell(size_t r, size_t c) const { return *cells.at(r).at(c); }

    /** Replace a cell at (r,c) with a clone of v.
     * @param r row index
     * @param c column index
     * @param v source cell to clone
     */
    void setCell(size_t r, size_t c, const CCell & v)
    {
        cells.at(r).at(c).reset(v.clone());
    }

    /**
     * Compute total width including borders.
     * @return width in characters
     */
    size_t getWidth()  const override
    {
        size_t w = 1;
        for (size_t c = 0; c < cols; ++c)
        {
            size_t cw = 0;
            for (size_t r = 0; r < rows; ++r)
                cw = max(cw, cells[r][c]->getWidth());
            w += cw + 1;
        }
        return w;
    }

    /**
     * Compute total height including borders.
     * @return height in lines
     */
    size_t getHeight() const override
    {
        size_t h = 1;
        for (size_t r = 0; r < rows; ++r)
        {
            size_t rh = 0;
            for (size_t c = 0; c < cols; ++c)
                rh = max(rh, cells[r][c]->getHeight());
            h += rh + 1;
        }
        return h;
    }

    /**
     * Render full table with ASCII borders.
     * @param width target width
     * @param height target height
     * @return block of text representing table
     */
    vector<string> printToOut(size_t width, size_t height) const override
    {
        //  Determine the maximum width of each column and maximum height of each row
        vector<size_t> colW(cols, 0), rowH(rows, 0);
        for (size_t r = 0; r < rows; ++r)
        {
            for (size_t c = 0; c < cols; ++c)
            {
                // cell’s natural dimensions
                size_t cw = cells[r][c]->getWidth();
                size_t ch = cells[r][c]->getHeight();
                // track the widest cell in this column
                colW[c] = max(colW[c], cw);
                // track the tallest cell in this row
                rowH[r] = max(rowH[r], ch);
            }
        }

        //  Render each individual cell into its own block of strings,
        //    sized to the column width and row height determined above
        vector<vector<vector<string>>> cellLines(
                rows, vector<vector<string>>(cols)
        );
        for (size_t r = 0; r < rows; ++r)
        {
            for (size_t c = 0; c < cols; ++c)
            {
                cellLines[r][c] = cells[r][c]->printToOut(colW[c], rowH[r]);
            }
        }

        //  Prepare output buffer and reserve the expected number of lines
        vector<string> out;
        out.reserve(height);

        //  Helper lambda to print a horizontal border line
        auto printBorder = [&]() {
            // start with corner
            string line = "+";
            // add dashes for each column, then a '+' corner
            for (size_t c = 0; c < cols; ++c)
            {
                line += string(colW[c], '-');
                line += '+';
            }
            // if the border is shorter than the full width, pad with spaces
            if (line.size() < width)
                line += string(width - line.size(), ' ');
            out.push_back(std::move(line));
        };

        //  Print the top border
        printBorder();

        //  For each row:
        //    a) for each line within the row’s height, assemble a line of text
        //    b) wrap each cell’s rendered line with '|' borders
        //    c) pad to full width if needed
        for (size_t r = 0; r < rows; ++r)
        {
            for (size_t ln = 0; ln < rowH[r]; ++ln)
            {
                string line = "|";
                for (size_t c = 0; c < cols; ++c)
                {
                    // append the ln-th line of the (r,c) cell
                    line += cellLines[r][c][ln];
                    line += '|';
                }
                if (line.size() < width)
                    line += string(width - line.size(), ' ');
                out.push_back(std::move(line));
            }
            // after finishing the row’s content, print its bottom border
            printBorder();
        }

        //  If the rendered table is shorter than the requested height,
        //    pad the remaining lines with empty spaces
        while (out.size() < height)
            out.emplace_back(width, ' ');

        return out;
    }

    /** Stream insertion prints the table to output stream. */
    friend ostream& operator<<(ostream & os, const CTable & t)
    {
        auto lines = t.printToOut(t.getWidth(), t.getHeight());
        for (auto & l : lines) os << l << '\n';
        return os;
    }

    /** Dispatch equality. */
    bool operator==(const CCell & b) const override
    {
        return b.equals(*this);
    }
    bool equals(const CEmpty & other) const override { return false; }
    bool equals(const CText  & other) const override { return false; }
    bool equals(const CImage & other) const override { return false; }
    bool equals(const CTable & other) const override
    {
        if (other.rows != rows || other.cols != cols) return false;
        for (size_t r = 0; r < rows; ++r)
            for (size_t c = 0; c < cols; ++c)
                if (*cells[r][c] != *other.cells[r][c])
                    return false;
        return true;
    }
//---------------------------------------------------------------------------------------------

private:
    size_t rows, cols; // Number of rows and columns
    vector<vector<unique_ptr<CCell>>> cells; // 2D vector of cell pointers
};
//---------------------------------------------------------------------------------------------


#ifndef __PROGTEST__
int main ()
{
    std::ostringstream oss;
    CTable t0 ( 3, 2 );
    t0 . setCell ( 0, 0, CText ( "Hello,\n"
                                 "Hello Kitty", CText::ALIGN_LEFT ) );
    t0 . setCell ( 1, 0, CText ( "Lorem ipsum dolor sit amet", CText::ALIGN_LEFT ) );
    t0 . setCell ( 2, 0, CText ( "Bye,\n"
                                 "Hello Kitty", CText::ALIGN_RIGHT ) );
    t0 . setCell ( 1, 1, CImage ()
            . addRow ( "###                   " )
            . addRow ( "#  #                  " )
            . addRow ( "#  # # ##   ###    ###" )
            . addRow ( "###  ##    #   #  #  #" )
            . addRow ( "#    #     #   #  #  #" )
            . addRow ( "#    #     #   #  #  #" )
            . addRow ( "#    #      ###    ###" )
            . addRow ( "                     #" )
            . addRow ( "                   ## " )
            . addRow ( "                      " )
            . addRow ( " #    ###   ###   #   " )
            . addRow ( "###  #   # #     ###  " )
            . addRow ( " #   #####  ###   #   " )
            . addRow ( " #   #         #  #   " )
            . addRow ( "  ##  ###   ###    ## " ) );
    t0 . setCell ( 2, 1, CEmpty () );
    oss . str ("");
    oss . clear ();
    oss << t0;
    assert ( oss . str () ==
             "+--------------------------+----------------------+\n"
             "|Hello,                    |                      |\n"
             "|Hello Kitty               |                      |\n"
             "+--------------------------+----------------------+\n"
             "|Lorem ipsum dolor sit amet|###                   |\n"
             "|                          |#  #                  |\n"
             "|                          |#  # # ##   ###    ###|\n"
             "|                          |###  ##    #   #  #  #|\n"
             "|                          |#    #     #   #  #  #|\n"
             "|                          |#    #     #   #  #  #|\n"
             "|                          |#    #      ###    ###|\n"
             "|                          |                     #|\n"
             "|                          |                   ## |\n"
             "|                          |                      |\n"
             "|                          | #    ###   ###   #   |\n"
             "|                          |###  #   # #     ###  |\n"
             "|                          | #   #####  ###   #   |\n"
             "|                          | #   #         #  #   |\n"
             "|                          |  ##  ###   ###    ## |\n"
             "+--------------------------+----------------------+\n"
             "|                      Bye,|                      |\n"
             "|               Hello Kitty|                      |\n"
             "+--------------------------+----------------------+\n" );
    t0 . setCell ( 0, 1, t0 . getCell ( 1, 1 ) );
    t0 . setCell ( 2, 1, CImage ()
            . addRow ( "*****   *      *  *      ******* ******  *" )
            . addRow ( "*    *  *      *  *      *            *  *" )
            . addRow ( "*    *  *      *  *      *           *   *" )
            . addRow ( "*    *  *      *  *      *****      *    *" )
            . addRow ( "****    *      *  *      *         *     *" )
            . addRow ( "*  *    *      *  *      *        *       " )
            . addRow ( "*   *   *      *  *      *       *       *" )
            . addRow ( "*    *    *****   ****** ******* ******  *" ) );
    dynamic_cast<CText &> ( t0 . getCell ( 1, 0 ) ) . setText ( "Lorem ipsum dolor sit amet,\n"
                                                                "consectetur adipiscing\n"
                                                                "elit. Curabitur scelerisque\n"
                                                                "lorem vitae lectus cursus,\n"
                                                                "vitae porta ante placerat. Class aptent taciti\n"
                                                                "sociosqu ad litora\n"
                                                                "torquent per\n"
                                                                "conubia nostra,\n"
                                                                "per inceptos himenaeos.\n"
                                                                "\n"
                                                                "Donec tincidunt augue\n"
                                                                "sit amet metus\n"
                                                                "pretium volutpat.\n"
                                                                "Donec faucibus,\n"
                                                                "ante sit amet\n"
                                                                "luctus posuere,\n"
                                                                "mauris tellus" );
    oss . str ("");
    oss . clear ();
    oss << t0;
    assert ( oss . str () ==
             "+----------------------------------------------+------------------------------------------+\n"
             "|Hello,                                        |          ###                             |\n"
             "|Hello Kitty                                   |          #  #                            |\n"
             "|                                              |          #  # # ##   ###    ###          |\n"
             "|                                              |          ###  ##    #   #  #  #          |\n"
             "|                                              |          #    #     #   #  #  #          |\n"
             "|                                              |          #    #     #   #  #  #          |\n"
             "|                                              |          #    #      ###    ###          |\n"
             "|                                              |                               #          |\n"
             "|                                              |                             ##           |\n"
             "|                                              |                                          |\n"
             "|                                              |           #    ###   ###   #             |\n"
             "|                                              |          ###  #   # #     ###            |\n"
             "|                                              |           #   #####  ###   #             |\n"
             "|                                              |           #   #         #  #             |\n"
             "|                                              |            ##  ###   ###    ##           |\n"
             "+----------------------------------------------+------------------------------------------+\n"
             "|Lorem ipsum dolor sit amet,                   |                                          |\n"
             "|consectetur adipiscing                        |          ###                             |\n"
             "|elit. Curabitur scelerisque                   |          #  #                            |\n"
             "|lorem vitae lectus cursus,                    |          #  # # ##   ###    ###          |\n"
             "|vitae porta ante placerat. Class aptent taciti|          ###  ##    #   #  #  #          |\n"
             "|sociosqu ad litora                            |          #    #     #   #  #  #          |\n"
             "|torquent per                                  |          #    #     #   #  #  #          |\n"
             "|conubia nostra,                               |          #    #      ###    ###          |\n"
             "|per inceptos himenaeos.                       |                               #          |\n"
             "|                                              |                             ##           |\n"
             "|Donec tincidunt augue                         |                                          |\n"
             "|sit amet metus                                |           #    ###   ###   #             |\n"
             "|pretium volutpat.                             |          ###  #   # #     ###            |\n"
             "|Donec faucibus,                               |           #   #####  ###   #             |\n"
             "|ante sit amet                                 |           #   #         #  #             |\n"
             "|luctus posuere,                               |            ##  ###   ###    ##           |\n"
             "|mauris tellus                                 |                                          |\n"
             "+----------------------------------------------+------------------------------------------+\n"
             "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
             "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
             "|                                              |*    *  *      *  *      *           *   *|\n"
             "|                                              |*    *  *      *  *      *****      *    *|\n"
             "|                                              |****    *      *  *      *         *     *|\n"
             "|                                              |*  *    *      *  *      *        *       |\n"
             "|                                              |*   *   *      *  *      *       *       *|\n"
             "|                                              |*    *    *****   ****** ******* ******  *|\n"
             "+----------------------------------------------+------------------------------------------+\n" );
    CTable t1 ( t0 );
    t1 . setCell ( 1, 0, CEmpty () );
    t1 . setCell ( 1, 1, CEmpty () );
    oss . str ("");
    oss . clear ();
    oss << t0;
    assert ( oss . str () ==
             "+----------------------------------------------+------------------------------------------+\n"
             "|Hello,                                        |          ###                             |\n"
             "|Hello Kitty                                   |          #  #                            |\n"
             "|                                              |          #  # # ##   ###    ###          |\n"
             "|                                              |          ###  ##    #   #  #  #          |\n"
             "|                                              |          #    #     #   #  #  #          |\n"
             "|                                              |          #    #     #   #  #  #          |\n"
             "|                                              |          #    #      ###    ###          |\n"
             "|                                              |                               #          |\n"
             "|                                              |                             ##           |\n"
             "|                                              |                                          |\n"
             "|                                              |           #    ###   ###   #             |\n"
             "|                                              |          ###  #   # #     ###            |\n"
             "|                                              |           #   #####  ###   #             |\n"
             "|                                              |           #   #         #  #             |\n"
             "|                                              |            ##  ###   ###    ##           |\n"
             "+----------------------------------------------+------------------------------------------+\n"
             "|Lorem ipsum dolor sit amet,                   |                                          |\n"
             "|consectetur adipiscing                        |          ###                             |\n"
             "|elit. Curabitur scelerisque                   |          #  #                            |\n"
             "|lorem vitae lectus cursus,                    |          #  # # ##   ###    ###          |\n"
             "|vitae porta ante placerat. Class aptent taciti|          ###  ##    #   #  #  #          |\n"
             "|sociosqu ad litora                            |          #    #     #   #  #  #          |\n"
             "|torquent per                                  |          #    #     #   #  #  #          |\n"
             "|conubia nostra,                               |          #    #      ###    ###          |\n"
             "|per inceptos himenaeos.                       |                               #          |\n"
             "|                                              |                             ##           |\n"
             "|Donec tincidunt augue                         |                                          |\n"
             "|sit amet metus                                |           #    ###   ###   #             |\n"
             "|pretium volutpat.                             |          ###  #   # #     ###            |\n"
             "|Donec faucibus,                               |           #   #####  ###   #             |\n"
             "|ante sit amet                                 |           #   #         #  #             |\n"
             "|luctus posuere,                               |            ##  ###   ###    ##           |\n"
             "|mauris tellus                                 |                                          |\n"
             "+----------------------------------------------+------------------------------------------+\n"
             "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
             "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
             "|                                              |*    *  *      *  *      *           *   *|\n"
             "|                                              |*    *  *      *  *      *****      *    *|\n"
             "|                                              |****    *      *  *      *         *     *|\n"
             "|                                              |*  *    *      *  *      *        *       |\n"
             "|                                              |*   *   *      *  *      *       *       *|\n"
             "|                                              |*    *    *****   ****** ******* ******  *|\n"
             "+----------------------------------------------+------------------------------------------+\n" );
    oss . str ("");
    oss . clear ();
    oss << t1;
    assert ( oss . str () ==
             "+-----------+------------------------------------------+\n"
             "|Hello,     |          ###                             |\n"
             "|Hello Kitty|          #  #                            |\n"
             "|           |          #  # # ##   ###    ###          |\n"
             "|           |          ###  ##    #   #  #  #          |\n"
             "|           |          #    #     #   #  #  #          |\n"
             "|           |          #    #     #   #  #  #          |\n"
             "|           |          #    #      ###    ###          |\n"
             "|           |                               #          |\n"
             "|           |                             ##           |\n"
             "|           |                                          |\n"
             "|           |           #    ###   ###   #             |\n"
             "|           |          ###  #   # #     ###            |\n"
             "|           |           #   #####  ###   #             |\n"
             "|           |           #   #         #  #             |\n"
             "|           |            ##  ###   ###    ##           |\n"
             "+-----------+------------------------------------------+\n"
             "+-----------+------------------------------------------+\n"
             "|       Bye,|*****   *      *  *      ******* ******  *|\n"
             "|Hello Kitty|*    *  *      *  *      *            *  *|\n"
             "|           |*    *  *      *  *      *           *   *|\n"
             "|           |*    *  *      *  *      *****      *    *|\n"
             "|           |****    *      *  *      *         *     *|\n"
             "|           |*  *    *      *  *      *        *       |\n"
             "|           |*   *   *      *  *      *       *       *|\n"
             "|           |*    *    *****   ****** ******* ******  *|\n"
             "+-----------+------------------------------------------+\n" );
    t1 = t0;
    t1 . setCell ( 0, 0, CEmpty () );
    t1 . setCell ( 1, 1, CImage ()
            . addRow ( "  ********                    " )
            . addRow ( " **********                   " )
            . addRow ( "**        **                  " )
            . addRow ( "**             **        **   " )
            . addRow ( "**             **        **   " )
            . addRow ( "***         ********  ********" )
            . addRow ( "****        ********  ********" )
            . addRow ( "****           **        **   " )
            . addRow ( "****           **        **   " )
            . addRow ( "****      **                  " )
            . addRow ( " **********                   " )
            . addRow ( "  ********                    " ) );
    oss . str ("");
    oss . clear ();
    oss << t0;
    assert ( oss . str () ==
             "+----------------------------------------------+------------------------------------------+\n"
             "|Hello,                                        |          ###                             |\n"
             "|Hello Kitty                                   |          #  #                            |\n"
             "|                                              |          #  # # ##   ###    ###          |\n"
             "|                                              |          ###  ##    #   #  #  #          |\n"
             "|                                              |          #    #     #   #  #  #          |\n"
             "|                                              |          #    #     #   #  #  #          |\n"
             "|                                              |          #    #      ###    ###          |\n"
             "|                                              |                               #          |\n"
             "|                                              |                             ##           |\n"
             "|                                              |                                          |\n"
             "|                                              |           #    ###   ###   #             |\n"
             "|                                              |          ###  #   # #     ###            |\n"
             "|                                              |           #   #####  ###   #             |\n"
             "|                                              |           #   #         #  #             |\n"
             "|                                              |            ##  ###   ###    ##           |\n"
             "+----------------------------------------------+------------------------------------------+\n"
             "|Lorem ipsum dolor sit amet,                   |                                          |\n"
             "|consectetur adipiscing                        |          ###                             |\n"
             "|elit. Curabitur scelerisque                   |          #  #                            |\n"
             "|lorem vitae lectus cursus,                    |          #  # # ##   ###    ###          |\n"
             "|vitae porta ante placerat. Class aptent taciti|          ###  ##    #   #  #  #          |\n"
             "|sociosqu ad litora                            |          #    #     #   #  #  #          |\n"
             "|torquent per                                  |          #    #     #   #  #  #          |\n"
             "|conubia nostra,                               |          #    #      ###    ###          |\n"
             "|per inceptos himenaeos.                       |                               #          |\n"
             "|                                              |                             ##           |\n"
             "|Donec tincidunt augue                         |                                          |\n"
             "|sit amet metus                                |           #    ###   ###   #             |\n"
             "|pretium volutpat.                             |          ###  #   # #     ###            |\n"
             "|Donec faucibus,                               |           #   #####  ###   #             |\n"
             "|ante sit amet                                 |           #   #         #  #             |\n"
             "|luctus posuere,                               |            ##  ###   ###    ##           |\n"
             "|mauris tellus                                 |                                          |\n"
             "+----------------------------------------------+------------------------------------------+\n"
             "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
             "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
             "|                                              |*    *  *      *  *      *           *   *|\n"
             "|                                              |*    *  *      *  *      *****      *    *|\n"
             "|                                              |****    *      *  *      *         *     *|\n"
             "|                                              |*  *    *      *  *      *        *       |\n"
             "|                                              |*   *   *      *  *      *       *       *|\n"
             "|                                              |*    *    *****   ****** ******* ******  *|\n"
             "+----------------------------------------------+------------------------------------------+\n" );
    oss . str ("");
    oss . clear ();
    oss << t1;
    assert ( oss . str () ==
             "+----------------------------------------------+------------------------------------------+\n"
             "|                                              |          ###                             |\n"
             "|                                              |          #  #                            |\n"
             "|                                              |          #  # # ##   ###    ###          |\n"
             "|                                              |          ###  ##    #   #  #  #          |\n"
             "|                                              |          #    #     #   #  #  #          |\n"
             "|                                              |          #    #     #   #  #  #          |\n"
             "|                                              |          #    #      ###    ###          |\n"
             "|                                              |                               #          |\n"
             "|                                              |                             ##           |\n"
             "|                                              |                                          |\n"
             "|                                              |           #    ###   ###   #             |\n"
             "|                                              |          ###  #   # #     ###            |\n"
             "|                                              |           #   #####  ###   #             |\n"
             "|                                              |           #   #         #  #             |\n"
             "|                                              |            ##  ###   ###    ##           |\n"
             "+----------------------------------------------+------------------------------------------+\n"
             "|Lorem ipsum dolor sit amet,                   |                                          |\n"
             "|consectetur adipiscing                        |                                          |\n"
             "|elit. Curabitur scelerisque                   |        ********                          |\n"
             "|lorem vitae lectus cursus,                    |       **********                         |\n"
             "|vitae porta ante placerat. Class aptent taciti|      **        **                        |\n"
             "|sociosqu ad litora                            |      **             **        **         |\n"
             "|torquent per                                  |      **             **        **         |\n"
             "|conubia nostra,                               |      ***         ********  ********      |\n"
             "|per inceptos himenaeos.                       |      ****        ********  ********      |\n"
             "|                                              |      ****           **        **         |\n"
             "|Donec tincidunt augue                         |      ****           **        **         |\n"
             "|sit amet metus                                |      ****      **                        |\n"
             "|pretium volutpat.                             |       **********                         |\n"
             "|Donec faucibus,                               |        ********                          |\n"
             "|ante sit amet                                 |                                          |\n"
             "|luctus posuere,                               |                                          |\n"
             "|mauris tellus                                 |                                          |\n"
             "+----------------------------------------------+------------------------------------------+\n"
             "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
             "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
             "|                                              |*    *  *      *  *      *           *   *|\n"
             "|                                              |*    *  *      *  *      *****      *    *|\n"
             "|                                              |****    *      *  *      *         *     *|\n"
             "|                                              |*  *    *      *  *      *        *       |\n"
             "|                                              |*   *   *      *  *      *       *       *|\n"
             "|                                              |*    *    *****   ****** ******* ******  *|\n"
             "+----------------------------------------------+------------------------------------------+\n" );
    CTable t2 ( 2, 2 );
    t2 . setCell ( 0, 0, CText ( "OOP", CText::ALIGN_LEFT ) );
    t2 . setCell ( 0, 1, CText ( "Encapsulation", CText::ALIGN_LEFT ) );
    t2 . setCell ( 1, 0, CText ( "Polymorphism", CText::ALIGN_LEFT ) );
    t2 . setCell ( 1, 1, CText ( "Inheritance", CText::ALIGN_LEFT ) );
    oss . str ("");
    oss . clear ();
    oss << t2;
    assert ( oss . str () ==
             "+------------+-------------+\n"
             "|OOP         |Encapsulation|\n"
             "+------------+-------------+\n"
             "|Polymorphism|Inheritance  |\n"
             "+------------+-------------+\n" );
    t1 . setCell ( 0, 0, t2 );
    dynamic_cast<CText &> ( t2 . getCell ( 0, 0 ) ) . setText ( "Object Oriented Programming" );
    oss . str ("");
    oss . clear ();
    oss << t2;
    assert ( oss . str () ==
             "+---------------------------+-------------+\n"
             "|Object Oriented Programming|Encapsulation|\n"
             "+---------------------------+-------------+\n"
             "|Polymorphism               |Inheritance  |\n"
             "+---------------------------+-------------+\n" );
    oss . str ("");
    oss . clear ();
    oss << t1;
    assert ( oss . str () ==
             "+----------------------------------------------+------------------------------------------+\n"
             "|+------------+-------------+                  |          ###                             |\n"
             "||OOP         |Encapsulation|                  |          #  #                            |\n"
             "|+------------+-------------+                  |          #  # # ##   ###    ###          |\n"
             "||Polymorphism|Inheritance  |                  |          ###  ##    #   #  #  #          |\n"
             "|+------------+-------------+                  |          #    #     #   #  #  #          |\n"
             "|                                              |          #    #     #   #  #  #          |\n"
             "|                                              |          #    #      ###    ###          |\n"
             "|                                              |                               #          |\n"
             "|                                              |                             ##           |\n"
             "|                                              |                                          |\n"
             "|                                              |           #    ###   ###   #             |\n"
             "|                                              |          ###  #   # #     ###            |\n"
             "|                                              |           #   #####  ###   #             |\n"
             "|                                              |           #   #         #  #             |\n"
             "|                                              |            ##  ###   ###    ##           |\n"
             "+----------------------------------------------+------------------------------------------+\n"
             "|Lorem ipsum dolor sit amet,                   |                                          |\n"
             "|consectetur adipiscing                        |                                          |\n"
             "|elit. Curabitur scelerisque                   |        ********                          |\n"
             "|lorem vitae lectus cursus,                    |       **********                         |\n"
             "|vitae porta ante placerat. Class aptent taciti|      **        **                        |\n"
             "|sociosqu ad litora                            |      **             **        **         |\n"
             "|torquent per                                  |      **             **        **         |\n"
             "|conubia nostra,                               |      ***         ********  ********      |\n"
             "|per inceptos himenaeos.                       |      ****        ********  ********      |\n"
             "|                                              |      ****           **        **         |\n"
             "|Donec tincidunt augue                         |      ****           **        **         |\n"
             "|sit amet metus                                |      ****      **                        |\n"
             "|pretium volutpat.                             |       **********                         |\n"
             "|Donec faucibus,                               |        ********                          |\n"
             "|ante sit amet                                 |                                          |\n"
             "|luctus posuere,                               |                                          |\n"
             "|mauris tellus                                 |                                          |\n"
             "+----------------------------------------------+------------------------------------------+\n"
             "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
             "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
             "|                                              |*    *  *      *  *      *           *   *|\n"
             "|                                              |*    *  *      *  *      *****      *    *|\n"
             "|                                              |****    *      *  *      *         *     *|\n"
             "|                                              |*  *    *      *  *      *        *       |\n"
             "|                                              |*   *   *      *  *      *       *       *|\n"
             "|                                              |*    *    *****   ****** ******* ******  *|\n"
             "+----------------------------------------------+------------------------------------------+\n" );
    assert ( t0 != t1 );
    assert ( !( t0 == t1 ) );
    assert ( t0 . getCell ( 1, 1 ) == t0 . getCell ( 0, 1 ) );
    assert ( ! ( t0 . getCell ( 1, 1 ) != t0 . getCell ( 0, 1 ) ) );
    assert ( t0 . getCell ( 0, 0 ) != t0 . getCell ( 0, 1 ) );
    assert ( ! ( t0 . getCell ( 0, 0 ) == t0 . getCell ( 0, 1 ) ) );
    t1 . setCell ( 0, 0, t1 );
    oss . str ("");
    oss . clear ();
    oss << t1;
    assert ( oss . str () ==
             "+-------------------------------------------------------------------------------------------+------------------------------------------+\n"
             "|+----------------------------------------------+------------------------------------------+|                                          |\n"
             "||+------------+-------------+                  |          ###                             ||                                          |\n"
             "|||OOP         |Encapsulation|                  |          #  #                            ||                                          |\n"
             "||+------------+-------------+                  |          #  # # ##   ###    ###          ||                                          |\n"
             "|||Polymorphism|Inheritance  |                  |          ###  ##    #   #  #  #          ||                                          |\n"
             "||+------------+-------------+                  |          #    #     #   #  #  #          ||                                          |\n"
             "||                                              |          #    #     #   #  #  #          ||                                          |\n"
             "||                                              |          #    #      ###    ###          ||                                          |\n"
             "||                                              |                               #          ||                                          |\n"
             "||                                              |                             ##           ||                                          |\n"
             "||                                              |                                          ||                                          |\n"
             "||                                              |           #    ###   ###   #             ||                                          |\n"
             "||                                              |          ###  #   # #     ###            ||                                          |\n"
             "||                                              |           #   #####  ###   #             ||                                          |\n"
             "||                                              |           #   #         #  #             ||          ###                             |\n"
             "||                                              |            ##  ###   ###    ##           ||          #  #                            |\n"
             "|+----------------------------------------------+------------------------------------------+|          #  # # ##   ###    ###          |\n"
             "||Lorem ipsum dolor sit amet,                   |                                          ||          ###  ##    #   #  #  #          |\n"
             "||consectetur adipiscing                        |                                          ||          #    #     #   #  #  #          |\n"
             "||elit. Curabitur scelerisque                   |        ********                          ||          #    #     #   #  #  #          |\n"
             "||lorem vitae lectus cursus,                    |       **********                         ||          #    #      ###    ###          |\n"
             "||vitae porta ante placerat. Class aptent taciti|      **        **                        ||                               #          |\n"
             "||sociosqu ad litora                            |      **             **        **         ||                             ##           |\n"
             "||torquent per                                  |      **             **        **         ||                                          |\n"
             "||conubia nostra,                               |      ***         ********  ********      ||           #    ###   ###   #             |\n"
             "||per inceptos himenaeos.                       |      ****        ********  ********      ||          ###  #   # #     ###            |\n"
             "||                                              |      ****           **        **         ||           #   #####  ###   #             |\n"
             "||Donec tincidunt augue                         |      ****           **        **         ||           #   #         #  #             |\n"
             "||sit amet metus                                |      ****      **                        ||            ##  ###   ###    ##           |\n"
             "||pretium volutpat.                             |       **********                         ||                                          |\n"
             "||Donec faucibus,                               |        ********                          ||                                          |\n"
             "||ante sit amet                                 |                                          ||                                          |\n"
             "||luctus posuere,                               |                                          ||                                          |\n"
             "||mauris tellus                                 |                                          ||                                          |\n"
             "|+----------------------------------------------+------------------------------------------+|                                          |\n"
             "||                                          Bye,|*****   *      *  *      ******* ******  *||                                          |\n"
             "||                                   Hello Kitty|*    *  *      *  *      *            *  *||                                          |\n"
             "||                                              |*    *  *      *  *      *           *   *||                                          |\n"
             "||                                              |*    *  *      *  *      *****      *    *||                                          |\n"
             "||                                              |****    *      *  *      *         *     *||                                          |\n"
             "||                                              |*  *    *      *  *      *        *       ||                                          |\n"
             "||                                              |*   *   *      *  *      *       *       *||                                          |\n"
             "||                                              |*    *    *****   ****** ******* ******  *||                                          |\n"
             "|+----------------------------------------------+------------------------------------------+|                                          |\n"
             "+-------------------------------------------------------------------------------------------+------------------------------------------+\n"
             "|Lorem ipsum dolor sit amet,                                                                |                                          |\n"
             "|consectetur adipiscing                                                                     |                                          |\n"
             "|elit. Curabitur scelerisque                                                                |        ********                          |\n"
             "|lorem vitae lectus cursus,                                                                 |       **********                         |\n"
             "|vitae porta ante placerat. Class aptent taciti                                             |      **        **                        |\n"
             "|sociosqu ad litora                                                                         |      **             **        **         |\n"
             "|torquent per                                                                               |      **             **        **         |\n"
             "|conubia nostra,                                                                            |      ***         ********  ********      |\n"
             "|per inceptos himenaeos.                                                                    |      ****        ********  ********      |\n"
             "|                                                                                           |      ****           **        **         |\n"
             "|Donec tincidunt augue                                                                      |      ****           **        **         |\n"
             "|sit amet metus                                                                             |      ****      **                        |\n"
             "|pretium volutpat.                                                                          |       **********                         |\n"
             "|Donec faucibus,                                                                            |        ********                          |\n"
             "|ante sit amet                                                                              |                                          |\n"
             "|luctus posuere,                                                                            |                                          |\n"
             "|mauris tellus                                                                              |                                          |\n"
             "+-------------------------------------------------------------------------------------------+------------------------------------------+\n"
             "|                                                                                       Bye,|*****   *      *  *      ******* ******  *|\n"
             "|                                                                                Hello Kitty|*    *  *      *  *      *            *  *|\n"
             "|                                                                                           |*    *  *      *  *      *           *   *|\n"
             "|                                                                                           |*    *  *      *  *      *****      *    *|\n"
             "|                                                                                           |****    *      *  *      *         *     *|\n"
             "|                                                                                           |*  *    *      *  *      *        *       |\n"
             "|                                                                                           |*   *   *      *  *      *       *       *|\n"
             "|                                                                                           |*    *    *****   ****** ******* ******  *|\n"
             "+-------------------------------------------------------------------------------------------+------------------------------------------+\n" );
    t1 . setCell ( 0, 0, t1 );
    oss . str ("");
    oss . clear ();
    oss << t1;
    assert ( oss . str () ==
             "+----------------------------------------------------------------------------------------------------------------------------------------+------------------------------------------+\n"
             "|+-------------------------------------------------------------------------------------------+------------------------------------------+|                                          |\n"
             "||+----------------------------------------------+------------------------------------------+|                                          ||                                          |\n"
             "|||+------------+-------------+                  |          ###                             ||                                          ||                                          |\n"
             "||||OOP         |Encapsulation|                  |          #  #                            ||                                          ||                                          |\n"
             "|||+------------+-------------+                  |          #  # # ##   ###    ###          ||                                          ||                                          |\n"
             "||||Polymorphism|Inheritance  |                  |          ###  ##    #   #  #  #          ||                                          ||                                          |\n"
             "|||+------------+-------------+                  |          #    #     #   #  #  #          ||                                          ||                                          |\n"
             "|||                                              |          #    #     #   #  #  #          ||                                          ||                                          |\n"
             "|||                                              |          #    #      ###    ###          ||                                          ||                                          |\n"
             "|||                                              |                               #          ||                                          ||                                          |\n"
             "|||                                              |                             ##           ||                                          ||                                          |\n"
             "|||                                              |                                          ||                                          ||                                          |\n"
             "|||                                              |           #    ###   ###   #             ||                                          ||                                          |\n"
             "|||                                              |          ###  #   # #     ###            ||                                          ||                                          |\n"
             "|||                                              |           #   #####  ###   #             ||                                          ||                                          |\n"
             "|||                                              |           #   #         #  #             ||          ###                             ||                                          |\n"
             "|||                                              |            ##  ###   ###    ##           ||          #  #                            ||                                          |\n"
             "||+----------------------------------------------+------------------------------------------+|          #  # # ##   ###    ###          ||                                          |\n"
             "|||Lorem ipsum dolor sit amet,                   |                                          ||          ###  ##    #   #  #  #          ||                                          |\n"
             "|||consectetur adipiscing                        |                                          ||          #    #     #   #  #  #          ||                                          |\n"
             "|||elit. Curabitur scelerisque                   |        ********                          ||          #    #     #   #  #  #          ||                                          |\n"
             "|||lorem vitae lectus cursus,                    |       **********                         ||          #    #      ###    ###          ||                                          |\n"
             "|||vitae porta ante placerat. Class aptent taciti|      **        **                        ||                               #          ||                                          |\n"
             "|||sociosqu ad litora                            |      **             **        **         ||                             ##           ||                                          |\n"
             "|||torquent per                                  |      **             **        **         ||                                          ||                                          |\n"
             "|||conubia nostra,                               |      ***         ********  ********      ||           #    ###   ###   #             ||                                          |\n"
             "|||per inceptos himenaeos.                       |      ****        ********  ********      ||          ###  #   # #     ###            ||                                          |\n"
             "|||                                              |      ****           **        **         ||           #   #####  ###   #             ||                                          |\n"
             "|||Donec tincidunt augue                         |      ****           **        **         ||           #   #         #  #             ||                                          |\n"
             "|||sit amet metus                                |      ****      **                        ||            ##  ###   ###    ##           ||          ###                             |\n"
             "|||pretium volutpat.                             |       **********                         ||                                          ||          #  #                            |\n"
             "|||Donec faucibus,                               |        ********                          ||                                          ||          #  # # ##   ###    ###          |\n"
             "|||ante sit amet                                 |                                          ||                                          ||          ###  ##    #   #  #  #          |\n"
             "|||luctus posuere,                               |                                          ||                                          ||          #    #     #   #  #  #          |\n"
             "|||mauris tellus                                 |                                          ||                                          ||          #    #     #   #  #  #          |\n"
             "||+----------------------------------------------+------------------------------------------+|                                          ||          #    #      ###    ###          |\n"
             "|||                                          Bye,|*****   *      *  *      ******* ******  *||                                          ||                               #          |\n"
             "|||                                   Hello Kitty|*    *  *      *  *      *            *  *||                                          ||                             ##           |\n"
             "|||                                              |*    *  *      *  *      *           *   *||                                          ||                                          |\n"
             "|||                                              |*    *  *      *  *      *****      *    *||                                          ||           #    ###   ###   #             |\n"
             "|||                                              |****    *      *  *      *         *     *||                                          ||          ###  #   # #     ###            |\n"
             "|||                                              |*  *    *      *  *      *        *       ||                                          ||           #   #####  ###   #             |\n"
             "|||                                              |*   *   *      *  *      *       *       *||                                          ||           #   #         #  #             |\n"
             "|||                                              |*    *    *****   ****** ******* ******  *||                                          ||            ##  ###   ###    ##           |\n"
             "||+----------------------------------------------+------------------------------------------+|                                          ||                                          |\n"
             "|+-------------------------------------------------------------------------------------------+------------------------------------------+|                                          |\n"
             "||Lorem ipsum dolor sit amet,                                                                |                                          ||                                          |\n"
             "||consectetur adipiscing                                                                     |                                          ||                                          |\n"
             "||elit. Curabitur scelerisque                                                                |        ********                          ||                                          |\n"
             "||lorem vitae lectus cursus,                                                                 |       **********                         ||                                          |\n"
             "||vitae porta ante placerat. Class aptent taciti                                             |      **        **                        ||                                          |\n"
             "||sociosqu ad litora                                                                         |      **             **        **         ||                                          |\n"
             "||torquent per                                                                               |      **             **        **         ||                                          |\n"
             "||conubia nostra,                                                                            |      ***         ********  ********      ||                                          |\n"
             "||per inceptos himenaeos.                                                                    |      ****        ********  ********      ||                                          |\n"
             "||                                                                                           |      ****           **        **         ||                                          |\n"
             "||Donec tincidunt augue                                                                      |      ****           **        **         ||                                          |\n"
             "||sit amet metus                                                                             |      ****      **                        ||                                          |\n"
             "||pretium volutpat.                                                                          |       **********                         ||                                          |\n"
             "||Donec faucibus,                                                                            |        ********                          ||                                          |\n"
             "||ante sit amet                                                                              |                                          ||                                          |\n"
             "||luctus posuere,                                                                            |                                          ||                                          |\n"
             "||mauris tellus                                                                              |                                          ||                                          |\n"
             "|+-------------------------------------------------------------------------------------------+------------------------------------------+|                                          |\n"
             "||                                                                                       Bye,|*****   *      *  *      ******* ******  *||                                          |\n"
             "||                                                                                Hello Kitty|*    *  *      *  *      *            *  *||                                          |\n"
             "||                                                                                           |*    *  *      *  *      *           *   *||                                          |\n"
             "||                                                                                           |*    *  *      *  *      *****      *    *||                                          |\n"
             "||                                                                                           |****    *      *  *      *         *     *||                                          |\n"
             "||                                                                                           |*  *    *      *  *      *        *       ||                                          |\n"
             "||                                                                                           |*   *   *      *  *      *       *       *||                                          |\n"
             "||                                                                                           |*    *    *****   ****** ******* ******  *||                                          |\n"
             "|+-------------------------------------------------------------------------------------------+------------------------------------------+|                                          |\n"
             "+----------------------------------------------------------------------------------------------------------------------------------------+------------------------------------------+\n"
             "|Lorem ipsum dolor sit amet,                                                                                                             |                                          |\n"
             "|consectetur adipiscing                                                                                                                  |                                          |\n"
             "|elit. Curabitur scelerisque                                                                                                             |        ********                          |\n"
             "|lorem vitae lectus cursus,                                                                                                              |       **********                         |\n"
             "|vitae porta ante placerat. Class aptent taciti                                                                                          |      **        **                        |\n"
             "|sociosqu ad litora                                                                                                                      |      **             **        **         |\n"
             "|torquent per                                                                                                                            |      **             **        **         |\n"
             "|conubia nostra,                                                                                                                         |      ***         ********  ********      |\n"
             "|per inceptos himenaeos.                                                                                                                 |      ****        ********  ********      |\n"
             "|                                                                                                                                        |      ****           **        **         |\n"
             "|Donec tincidunt augue                                                                                                                   |      ****           **        **         |\n"
             "|sit amet metus                                                                                                                          |      ****      **                        |\n"
             "|pretium volutpat.                                                                                                                       |       **********                         |\n"
             "|Donec faucibus,                                                                                                                         |        ********                          |\n"
             "|ante sit amet                                                                                                                           |                                          |\n"
             "|luctus posuere,                                                                                                                         |                                          |\n"
             "|mauris tellus                                                                                                                           |                                          |\n"
             "+----------------------------------------------------------------------------------------------------------------------------------------+------------------------------------------+\n"
             "|                                                                                                                                    Bye,|*****   *      *  *      ******* ******  *|\n"
             "|                                                                                                                             Hello Kitty|*    *  *      *  *      *            *  *|\n"
             "|                                                                                                                                        |*    *  *      *  *      *           *   *|\n"
             "|                                                                                                                                        |*    *  *      *  *      *****      *    *|\n"
             "|                                                                                                                                        |****    *      *  *      *         *     *|\n"
             "|                                                                                                                                        |*  *    *      *  *      *        *       |\n"
             "|                                                                                                                                        |*   *   *      *  *      *       *       *|\n"
             "|                                                                                                                                        |*    *    *****   ****** ******* ******  *|\n"
             "+----------------------------------------------------------------------------------------------------------------------------------------+------------------------------------------+\n" );

    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
