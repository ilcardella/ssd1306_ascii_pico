/*

  This is a port of the Arduino library for the Raspberry Pico platform

 * Arduino SSD1306Ascii Library
 * Copyright (C) 2015 by William Greiman
 *
 * This file is part of the Arduino SSD1306Ascii Library
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Arduino SSD1306Ascii Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
/**
 * @file SSD1306Ascii.h
 * @brief Base class for ssd1306 displays.
 */
#ifndef SSD1306Ascii_h
#define SSD1306Ascii_h
#include "Print.h"
#include "SSD1306init.h"
#include "fonts/allFonts.h"
#include <pico/stdlib.h>
//------------------------------------------------------------------------------
/** SSD1306Ascii version */
#define SDD1306_ASCII_VERSION 1.3.0
//------------------------------------------------------------------------------
// Configuration options.
/** Set Scrolling mode for newline.
 *
 * If INCLUDE_SCROLLING is defined to be zero, newline will not scroll
 * the display and code for scrolling will not be included.  This option
 * will save some code space and three bytes of RAM.
 *
 * If INCLUDE_SCROLLING is nonzero, the scroll feature will included.
 */
#define INCLUDE_SCROLLING 1

/** Initial scroll mode, SCROLL_MODE_OFF,
    SCROLL_MODE_AUTO, or SCROLL_MODE_APP. */
#define INITIAL_SCROLL_MODE SCROLL_MODE_OFF

/** Dimension of TickerState pointer queue */
#define TICKER_QUEUE_DIM 6

/** Use larger faster I2C code. */
#define OPTIMIZE_I2C 1

//------------------------------------------------------------------------------
// Values for setScrolMode(uint8_t mode)
/** Newline will not scroll the display or RAM window. */
#define SCROLL_MODE_OFF 0
/** Newline will scroll both the display and RAM windows. */
#define SCROLL_MODE_AUTO 1
/** Newline scrolls the RAM window. The app scrolls the display window. */
#define SCROLL_MODE_APP 2
//------------------------------------------------------------------------------
// Values for writeDisplay() mode parameter.
/** Write to Command register. */
#define SSD1306_MODE_CMD 0
/** Write one byte to display RAM. */
#define SSD1306_MODE_RAM 1
/** Write to display RAM with possible buffering. */
#define SSD1306_MODE_RAM_BUF 2
//------------------------------------------------------------------------------
/**
 * @brief Reset the display controller.
 *
 * @param[in] rst Reset pin number.
 */
inline void oledReset(uint8_t rst)
{
    gpio_init(rst);
    gpio_set_dir(rst, true);
    gpio_put(rst, 0);
    sleep_ms(10);
    gpio_put(rst, 1);
    sleep_ms(10);
}
//------------------------------------------------------------------------------
GLCDFONTDECL(scaledNibble) = {0X00, 0X03, 0X0C, 0X0F, 0X30, 0X33, 0X3C, 0X3F,
                              0XC0, 0XC3, 0XCC, 0XCF, 0XF0, 0XF3, 0XFC, 0XFF};
//------------------------------------------------------------------------------
/**
 * @struct TickerState
 * @brief ticker status
 */
struct TickerState
{
    const char *queue[TICKER_QUEUE_DIM]; ///< Queue of text pointers.
    uint8_t nQueue = 0;                  ///< Count of pointers in queue.
    const uint8_t *font = nullptr;       ///< Font for ticker.
    bool mag2X;                          ///< Use mag2X if true.
    uint8_t row;                         ///< Row for ticker
    uint8_t bgnCol;                      ///< Begin column of ticker.
    uint8_t endCol;                      ///< End column of ticker.
    bool init;                           ///< clear and initialize display area if true.
    uint8_t col;                         ///< Column for start of displayed text.
    uint8_t skip;                        ///< Number of pixels to skip in first character.
    /// @return Count of free queue slots.
    uint8_t queueFree()
    {
        return TICKER_QUEUE_DIM - nQueue;
    }
    /// @return Count of used queue slots.
    uint8_t queueUsed()
    {
        return nQueue;
    }
};
//------------------------------------------------------------------------------
/**
 * @class SSD1306Ascii
 * @brief SSD1306 base class
 */
class SSD1306Ascii : public Print
{
  public:
    SSD1306Ascii()
    {
    }
#if INCLUDE_SCROLLING
    //------------------------------------------------------------------------------
    /**
     * @return the RAM page for top of the RAM window.
     */
    uint8_t pageOffset() const
    {
        return m_pageOffset;
    }
    /**
     * @return the display line for pageOffset.
     */
    uint8_t pageOffsetLine() const
    {
        return 8 * m_pageOffset;
    }
    /**
     * @brief Scroll the Display window.
     *
     * @param[in] lines Number of lines to scroll the window.
     */
    void scrollDisplay(int8_t lines)
    {
        setStartLine(m_startLine + lines);
    }
    /**
     * @brief Scroll the RAM window.
     *
     * @param[in] rows Number of rows to scroll the window.
     */
    void scrollMemory(int8_t rows)
    {
        setPageOffset(m_pageOffset + rows);
    }
    /**
     * @return true if the first display line is equal to the
     *         start of the RAM window.
     */
    bool scrollIsSynced() const
    {
        return startLine() == pageOffsetLine();
    }
    /**
     * @brief Set page offset.
     *
     * @param[in] page the RAM page for start of the RAM window
     */
    void setPageOffset(uint8_t page)
    {
        m_pageOffset = page & 7;
        setRow(m_row);
    }
    /**
     * @brief Enable or disable scroll mode. Deprecated use setScrollMode().
     *
     * @param[in] enable true enable scroll on newline false disable scroll.
     */
    void setScroll(bool enable) __attribute__((deprecated("use setScrollMode")))
    {
        setScrollMode(enable ? SCROLL_MODE_AUTO : SCROLL_MODE_OFF);
    }
    /**
     * @brief Set scroll mode.
     *
     * @param[in] mode One of the following.
     *
     * SCROLL_MODE_OFF - newline will not scroll the display or RAM window.
     *
     * SCROLL_MODE_AUTO - newline will scroll both the display and RAM windows.
     *
     * SCROLL_MODE_APP - newline scrolls the RAM window.
     *                   The app scrolls the display window.
     */
    void setScrollMode(uint8_t mode)
    {
        m_scrollMode = mode;
    }
    /**
     * @brief Set the display start line register.
     *
     * @param[in] line RAM line to be mapped to first display line.
     */
    void setStartLine(uint8_t line)
    {
        m_startLine = line & 0X3F;
        ssd1306WriteCmd(SSD1306_SETSTARTLINE | m_startLine);
    }
    /**
     * @return the display startline.
     */
    uint8_t startLine() const
    {
        return m_startLine;
    }
#endif // INCLUDE_SCROLLING
    //----------------------------------------------------------------------------
    /**
     * @brief Determine the spacing of a character. Spacing is width + space.
     *
     * @param[in] c Character code.
     * @return Spacing of the character in pixels.
     */
    uint8_t charSpacing(uint8_t c)
    {
        return charWidth(c) + letterSpacing();
    }
    /**
     * @brief Determine the width of a character.
     *
     * @param[in] c Character code.
     * @return Width of the character in pixels.
     */
    uint8_t charWidth(uint8_t c) const
    {
        if (!m_font)
        {
            return 0;
        }
        uint8_t first = readFontByte(m_font + FONT_FIRST_CHAR);
        uint8_t count = readFontByte(m_font + FONT_CHAR_COUNT);
        if (c < first || c >= (first + count))
        {
            return 0;
        }
        if (fontSize() > 1)
        {
            // Proportional font.
            return m_magFactor * readFontByte(m_font + FONT_WIDTH_TABLE + c - first);
        }
        // Fixed width font.
        return m_magFactor * readFontByte(m_font + FONT_WIDTH);
    }
    /**
     * @brief Clear the display and set the cursor to (0, 0).
     */
    void clear()
    {
#if INCLUDE_SCROLLING
        m_pageOffset = 0;
        setStartLine(0);
#endif // INCLUDE_SCROLLING
        clear(0, displayWidth() - 1, 0, displayRows() - 1);
    }
    /**
     * @brief Clear a region of the display.
     *
     * @param[in] c0 Starting column.
     * @param[in] c1 Ending column.
     * @param[in] r0 Starting row;
     * @param[in] r1 Ending row;
     * @note The final cursor position will be (c0, r0).
     */
    void clear(uint8_t c0, uint8_t c1, uint8_t r0, uint8_t r1)
    {
        // Cancel skip character pixels.
        m_skip = 0;

        // Insure only rows on display will be cleared.
        if (r1 >= displayRows())
            r1 = displayRows() - 1;

        for (uint8_t r = r0; r <= r1; r++)
        {
            setCursor(c0, r);
            for (uint8_t c = c0; c <= c1; c++)
            {
                // Insure clear() writes zero. result is (m_invertMask^m_invertMask).
                ssd1306WriteRamBuf(m_invertMask);
            }
        }
        setCursor(c0, r0);
    }
    /**
     * @brief Clear a field of n fieldWidth() characters.
     *
     * @param[in] col Field start column.
     *
     * @param[in] row Field start row.
     *
     * @param[in] n Number of characters in the field.
     *
     */
    void clearField(uint8_t col, uint8_t row, uint8_t n)
    {
        clear(col, col + fieldWidth(n) - 1, row, row + fontRows() - 1);
    }
    /**
     * @brief Clear the display to the end of the current line.
     * @note The number of rows cleared will be determined by the height
     *       of the current font.
     * @note The cursor will be returned to the original position.
     */
    void clearToEOL()
    {
        clear(m_col, displayWidth() - 1, m_row, m_row + fontRows() - 1);
    }
    /**
     * @return The current column in pixels.
     */
    uint8_t col() const
    {
        return m_col;
    }
    /**
     * @return The display hight in pixels.
     */
    uint8_t displayHeight() const
    {
        return m_displayHeight;
    }
    /**
     * @brief Set display to normal or 180 degree remap mode.
     *
     * @param[in] mode true for normal mode, false for remap mode.
     *
     * @note Adafruit and many ebay displays use remap mode.
     *       Use normal mode to rotate these displays 180 degrees.
     */
    void displayRemap(bool mode)
    {
        ssd1306WriteCmd(mode ? SSD1306_SEGREMAP : SSD1306_SEGREMAP | 1);
        ssd1306WriteCmd(mode ? SSD1306_COMSCANINC : SSD1306_COMSCANDEC);
    }
    /**
     * @return The display height in rows with eight pixels to a row.
     */
    uint8_t displayRows() const
    {
        return m_displayHeight / 8;
    }
    /**
     * @return The display width in pixels.
     */
    uint8_t displayWidth() const
    {
        return m_displayWidth;
    }
    /**
     * @brief Width of a field in pixels.
     *
     * @param[in] n Number of characters in the field.
     *
     * @return Width of the field.
     */
    size_t fieldWidth(uint8_t n)
    {
        return n * (fontWidth() + letterSpacing());
    }
    /**
     * @return The current font pointer.
     */
    const uint8_t *font() const
    {
        return m_font;
    }
    /**
     * @return The count of characters in a font.
     */
    uint8_t fontCharCount() const
    {
        return m_font ? readFontByte(m_font + FONT_CHAR_COUNT) : 0;
    }
    /**
     * @return The first character in a font.
     */
    char fontFirstChar() const
    {
        return m_font ? readFontByte(m_font + FONT_FIRST_CHAR) : 0;
    }
    /**
     * @return The current font height in pixels.
     */
    uint8_t fontHeight() const
    {
        return m_font ? m_magFactor * readFontByte(m_font + FONT_HEIGHT) : 0;
    }
    /**
     * @return The number of eight pixel rows required to display a character
     *    in the current font.
     */
    uint8_t fontRows() const
    {
        return m_font ? m_magFactor * ((readFontByte(m_font + FONT_HEIGHT) + 7) / 8) : 0;
    }
    /**
     * @return The maximum width of characters in the current font.
     */
    uint8_t fontWidth() const
    {
        return m_font ? m_magFactor * readFontByte(m_font + FONT_WIDTH) : 0;
    }
    /**
     * @brief Set the cursor position to (0, 0).
     */
    void home()
    {
        setCursor(0, 0);
    }
    /**
     * @brief Initialize the display controller.
     *
     * @param[in] dev A display initialization structure.
     */
    void init(const DevType *dev)
    {
        m_col = 0;
        m_row = 0;
#ifdef __AVR__
        const uint8_t *table = (const uint8_t *)pgm_read_word(&dev->initcmds);
#else  // __AVR__
        const uint8_t *table = dev->initcmds;
#endif // __AVR
        uint8_t size = readFontByte(&dev->initSize);
        m_displayWidth = readFontByte(&dev->lcdWidth);
        m_displayHeight = readFontByte(&dev->lcdHeight);
        m_colOffset = readFontByte(&dev->colOffset);
        for (uint8_t i = 0; i < size; i++)
        {
            ssd1306WriteCmd(readFontByte(table + i));
        }
        clear();
    }
    /**
     * @brief Set pixel mode for for entire display.
     *
     * @param[in] invert Inverted display if true or normal display if false.
     */
    void invertDisplay(bool invert)
    {
        ssd1306WriteCmd(invert ? SSD1306_INVERTDISPLAY : SSD1306_NORMALDISPLAY);
    }
    /**
     * @return invert mode.
     */
    bool invertMode() const
    {
        return !!m_invertMask;
    }
    /**
     * @brief Set invert mode for write/print.
     *
     * @param[in] mode Invert pixels if true and use normal mode if false.
     */
    void setInvertMode(bool mode)
    {
        m_invertMask = mode ? 0XFF : 0;
    }
    /**
     * @return letter-spacing in pixels with magnification factor.
     */
    uint8_t letterSpacing() const
    {
        return m_magFactor * m_letterSpacing;
    }
    /**
     * @return The character magnification factor.
     */
    uint8_t magFactor() const
    {
        return m_magFactor;
    }
    /**
     * @return the current row number with eight pixels to a row.
     */
    uint8_t row() const
    {
        return m_row;
    }
    /**
     * @brief Set the character magnification factor to one.
     */
    void set1X()
    {
        m_magFactor = 1;
    }
    /**
     * @brief Set the character magnification factor to two.
     */
    void set2X()
    {
        m_magFactor = 2;
    }
    /**
     * @brief Set the current column number.
     *
     * @param[in] col The desired column number in pixels.
     */
    void setCol(uint8_t col)
    {
        if (col < m_displayWidth)
        {
            m_col = col;
            col += m_colOffset;
            ssd1306WriteCmd(SSD1306_SETLOWCOLUMN | (col & 0XF));
            ssd1306WriteCmd(SSD1306_SETHIGHCOLUMN | (col >> 4));
        }
    }
    /**
     * @brief Set the display contrast.
     *
     * @param[in] value The contrast level in th range 0 to 255.
     */
    void setContrast(uint8_t value)
    {
        ssd1306WriteCmd(SSD1306_SETCONTRAST);
        ssd1306WriteCmd(value);
    }
    /**
     * @brief Set the cursor position.
     *
     * @param[in] col The column number in pixels.
     * @param[in] row the row number in eight pixel rows.
     */
    void setCursor(uint8_t col, uint8_t row)
    {
        setCol(col);
        setRow(row);
    }
    /**
     * @brief Set the current font.
     *
     * @param[in] font Pointer to a font table.
     */
    void setFont(const uint8_t *font)
    {
        m_font = font;
        if (font && fontSize() == 1)
        {
            m_letterSpacing = 0;
        }
        else
        {
            m_letterSpacing = 1;
        }
    }
    /**
     * @brief Set letter-spacing.  setFont() will restore default letter-spacing.
     *
     * @param[in] pixels letter-spacing in pixels before magnification.
     */
    void setLetterSpacing(uint8_t pixels)
    {
        m_letterSpacing = pixels;
    }
    /**
     * @brief Set the current row number.
     *
     * @param[in] row the row number in eight pixel rows.
     */
    void setRow(uint8_t row)
    {
        if (row < displayRows())
        {
            m_row = row;
#if INCLUDE_SCROLLING
            ssd1306WriteCmd(SSD1306_SETSTARTPAGE | ((m_row + m_pageOffset) & 7));
#else  // INCLUDE_SCROLLING
            ssd1306WriteCmd(SSD1306_SETSTARTPAGE | m_row);
#endif // INCLUDE_SCROLLING
        }
    }
    /**
     * @brief Write a command byte to the display controller.
     *
     * @param[in] c The command byte.
     * @note The byte will immediately be sent to the controller.
     */
    void ssd1306WriteCmd(uint8_t c)
    {
        writeDisplay(c, SSD1306_MODE_CMD);
    }
    /**
     * @brief Write a byte to RAM in the display controller.
     *
     * @param[in] c The data byte.
     * @note The byte will immediately be sent to the controller.
     */
    void ssd1306WriteRam(uint8_t c)
    {
        if (m_col < m_displayWidth)
        {
            writeDisplay(c ^ m_invertMask, SSD1306_MODE_RAM);
            m_col++;
        }
    }
    /**
     * @brief Write a byte to RAM in the display controller.
     *
     * @param[in] c The data byte.
     * @note The byte may be buffered until a call to ssd1306WriteCmd
     *       or ssd1306WriteRam.
     */
    void ssd1306WriteRamBuf(uint8_t c)
    {
        if (m_skip)
        {
            m_skip--;
        }
        else if (m_col < m_displayWidth)
        {
            writeDisplay(c ^ m_invertMask, SSD1306_MODE_RAM_BUF);
            m_col++;
        }
    }
    /**
     * @brief Skip leading pixels writing characters to display display RAM.
     *
     * @param[in] n Number of pixels to skip.
     */
    void skipColumns(uint8_t n)
    {
        m_skip = n;
    }
    /**
     * @brief Character width.
     *
     * @param[in] str The pointer to string.
     * @return the width of the string in pixels.
     */
    size_t strWidth(const char *str) const
    {
        size_t sw = 0;
        while (*str)
        {
            uint8_t cw = charWidth(*str++);
            if (cw == 0)
            {
                return 0;
            }
            sw += cw + letterSpacing();
        }
        return sw;
    }
    /**
     * @brief Initialize TickerState struct and clear ticker field.
     *
     * @param[in,out] state Ticker state.
     * @param[in] font to be displayed.
     * @param[in] row Row for ticker.
     * @param[in] mag2X set magFactor to two if true.
     * @param[in] bgnCol First column of ticker. Default is zero.
     * @param[in] endCol Last column of ticker. Default is last column of display.
     */
    void tickerInit(TickerState *state, const uint8_t *font, uint8_t row,
                    bool mag2X = false, uint8_t bgnCol = 0, uint8_t endCol = 255)
    {
        state->font = font;
        state->row = row;
        state->mag2X = mag2X;
        state->bgnCol = bgnCol;
        state->endCol = endCol < m_displayWidth ? endCol : m_displayWidth - 1;
        state->nQueue = 0;
    }
    /**
     *  @brief Add text pointer to display queue.
     *
     * @param[in,out] state Ticker state.
     * @param[in] str Pointer to String object. Clear queue if nullptr.
     * @return false if queue is full else true.
     */
    // TODO this overload is commented out because I need to figure out how to use strings
    // bool tickerText(TickerState *state, const std::string &str)
    // {
    //     return tickerText(state, str ? str.c_str() : nullptr);
    // }
    /**
     *  @brief Add text pointer to display queue.
     *
     * @param[in,out] state Ticker state.
     * @param[in] text Pointer to C string.  Clear queue if nullptr.
     * @return false if queue is full else true.
     */
    bool tickerText(TickerState *state, const char *text)
    {
        if (!text)
        {
            state->nQueue = 0;
            return true;
        }
        if (state->nQueue >= TICKER_QUEUE_DIM)
        {
            return false;
        }
        if (state->nQueue == 0)
        {
            state->init = true;
        }
        state->queue[state->nQueue++] = text;
        return true;
    }
    /**
     * @brief Advance ticker by one pixel.
     *
     * @param[in,out] state Ticker state.
     *
     * @return Number of entries in text pointer queue.
     */
    int8_t tickerTick(TickerState *state)
    {
        if (!state->font)
        {
            return -1;
        }
        if (!state->nQueue)
        {
            return 0;
        }
        setFont(state->font);
        m_magFactor = state->mag2X ? 2 : 1;
        if (state->init)
        {
            clear(state->bgnCol, state->endCol, state->row, state->row + fontRows() - 1);
            state->col = state->endCol;
            state->skip = 0;
            state->init = false;
        }
        // Adjust display width to truncate pixels after endCol.  Find better way?
        uint8_t save = m_displayWidth;
        m_displayWidth = state->endCol + 1;

        // Skip pixels before bgnCol.
        skipColumns(state->skip);
        setCursor(state->col, state->row);
        for (uint8_t i = 0; i < state->nQueue; i++)
        {
            const char *str = state->queue[i];
            while (*str && m_col <= state->endCol)
            {
                write(*str++);
            }
            if (m_col > state->endCol)
            {
                break;
            }
        }
        if (m_col <= state->endCol)
        {
            clear(m_col, m_col, state->row, state->row + fontRows() - 1);
        }
        // Restore display width.
        m_displayWidth = save;

        if (state->nQueue == 1 && *state->queue[0] == 0)
        {
            state->nQueue = 0;
            return 0;
        }
        if (state->col > state->bgnCol)
        {
            state->col--;
        }
        else
        {
            state->skip++;
            if (state->skip >= charSpacing(*state->queue[0]))
            {
                state->skip = 0;
                state->queue[0]++;
                if (*state->queue[0] == 0 && state->nQueue > 1)
                {
                    state->nQueue--;
                    for (uint8_t i = 0; i < state->nQueue; i++)
                    {
                        state->queue[i] = state->queue[i + 1];
                    }
                }
            }
        }
        return state->nQueue;
    }
    /**
     * @brief Display a character.
     *
     * @param[in] c The character to display.
     * @return one for success else zero.
     */
    size_t write(uint8_t ch)
    {
        if (!m_font)
        {
            return 0;
        }
        uint8_t w = readFontByte(m_font + FONT_WIDTH);
        uint8_t h = readFontByte(m_font + FONT_HEIGHT);
        uint8_t nr = (h + 7) / 8;
        uint8_t first = readFontByte(m_font + FONT_FIRST_CHAR);
        uint8_t count = readFontByte(m_font + FONT_CHAR_COUNT);
        const uint8_t *base = m_font + FONT_WIDTH_TABLE;

        if (ch < first || ch >= (first + count))
        {
            if (ch == '\r')
            {
                setCol(0);
                return 1;
            }
            if (ch == '\n')
            {
                setCol(0);
                uint8_t fr = m_magFactor * nr;
#if INCLUDE_SCROLLING
                uint8_t dr = displayRows();
                uint8_t tmpRow = m_row + fr;
                int8_t delta = tmpRow + fr - dr;
                if (m_scrollMode == SCROLL_MODE_OFF || delta <= 0)
                {
                    setRow(tmpRow);
                }
                else
                {
                    m_pageOffset = (m_pageOffset + delta) & 7;
                    m_row = dr - fr;
                    // Cursor will be positioned by clearToEOL.
                    clearToEOL();
                    if (m_scrollMode == SCROLL_MODE_AUTO)
                    {
                        setStartLine(8 * m_pageOffset);
                    }
                }
#else  // INCLUDE_SCROLLING
                setRow(m_row + fr);
#endif // INCLUDE_SCROLLING
                return 1;
            }
            return 0;
        }
        ch -= first;
        uint8_t s = letterSpacing();
        uint8_t thieleShift = 0;
        if (fontSize() < 2)
        {
            // Fixed width font.
            base += nr * w * ch;
        }
        else
        {
            if (h & 7)
            {
                thieleShift = 8 - (h & 7);
            }
            uint16_t index = 0;
            for (uint8_t i = 0; i < ch; i++)
            {
                index += readFontByte(base + i);
            }
            w = readFontByte(base + ch);
            base += nr * index + count;
        }
        uint8_t scol = m_col;
        uint8_t srow = m_row;
        uint8_t skip = m_skip;
        for (uint8_t r = 0; r < nr; r++)
        {
            for (uint8_t m = 0; m < m_magFactor; m++)
            {
                skipColumns(skip);
                if (r || m)
                {
                    setCursor(scol, m_row + 1);
                }
                for (uint8_t c = 0; c < w; c++)
                {
                    uint8_t b = readFontByte(base + c + r * w);
                    if (thieleShift && (r + 1) == nr)
                    {
                        b >>= thieleShift;
                    }
                    if (m_magFactor == 2)
                    {
                        b = m ? b >> 4 : b & 0XF;
                        b = readFontByte(scaledNibble + b);
                        ssd1306WriteRamBuf(b);
                    }
                    ssd1306WriteRamBuf(b);
                }
                for (uint8_t i = 0; i < s; i++)
                {
                    ssd1306WriteRamBuf(0);
                }
            }
        }
        setRow(srow);
        return 1;
    }

  protected:
    uint16_t fontSize() const
    {
        return (readFontByte(m_font) << 8) | readFontByte(m_font + 1);
    }
    virtual void writeDisplay(uint8_t b, uint8_t mode) = 0;
    uint8_t m_col;           // Cursor column.
    uint8_t m_row;           // Cursor RAM row.
    uint8_t m_displayWidth;  // Display width.
    uint8_t m_displayHeight; // Display height.
    uint8_t m_colOffset;     // Column offset RAM to SEG.
    uint8_t m_letterSpacing; // Letter-spacing in pixels.
#if INCLUDE_SCROLLING
    uint8_t m_startLine;                        // Top line of display
    uint8_t m_pageOffset;                       // Top page of RAM window.
    uint8_t m_scrollMode = INITIAL_SCROLL_MODE; // Scroll mode for newline.
#endif                                          // INCLUDE_SCROLLING
    uint8_t m_skip = 0;
    const uint8_t *m_font = nullptr; // Current font.
    uint8_t m_invertMask = 0;        // font invert mask
    uint8_t m_magFactor = 1;         // Magnification factor.
};
#endif // SSD1306Ascii_h
