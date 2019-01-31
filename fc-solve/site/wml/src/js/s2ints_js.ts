export function find_index__board_string_to_ints(content: string): number[] {
    if (content.length > 0 && content.slice(-1) !== "\n") {
        content += "\n";
    }

    const rank_s = "A23456789TJQK";
    const rank_re = "[" + rank_s + "]";
    const suit_s = "CSHD";
    const suit_re = "[" + suit_s + "]";

    const card_re = rank_re + suit_re;
    const card_re_paren = "(" + card_re + ")";

    function string_repeat_for_transcrypt(s, n) {
        if (n === 0) return "";
        return s + string_repeat_for_transcrypt(s, n - 1);
    }

    function make_line(n) {
        return (
            ":?[ \t]*" +
            card_re_paren +
            string_repeat_for_transcrypt("[ \t]+" + card_re_paren, n - 1) +
            "[ \t]*\n"
        );
    }

    const complete_re =
        "^" +
        string_repeat_for_transcrypt(make_line(7), 4) +
        string_repeat_for_transcrypt(make_line(6), 4) +
        "\\s*$";

    const m = new RegExp(complete_re).exec(content);
    if (!m) {
        throw "Could not match.";
    }

    const cards = [
        "AC",
        "AD",
        "AH",
        "AS",
        "2C",
        "2D",
        "2H",
        "2S",
        "3C",
        "3D",
        "3H",
        "3S",
        "4C",
        "4D",
        "4H",
        "4S",
        "5C",
        "5D",
        "5H",
        "5S",
        "6C",
        "6D",
        "6H",
        "6S",
        "7C",
        "7D",
        "7H",
        "7S",
        "8C",
        "8D",
        "8H",
        "8S",
        "9C",
        "9D",
        "9H",
        "9S",
        "TC",
        "TD",
        "TH",
        "TS",
        "JC",
        "JD",
        "JH",
        "JS",
        "QC",
        "QD",
        "QH",
        "QS",
        "KC",
        "KD",
        "KH",
        "KS",
    ];

    // Reverse shuffle:
    const ints: number[] = [];
    let n = 4 * 13 - 1;
    const limit = n;
    for (let i = 0; i < limit; ++i) {
        const col = i >> 3;
        const row = i % 8;
        const s = m[1 + col + (row >= 4 ? 4 * 7 + (row - 4) * 6 : row * 7)];
        function _perl_range(start: number, end: number): number[] {
            const ret: number[] = [];

            for (let i = start; i <= end; i++) {
                ret.push(i);
            }

            return ret;
        }
        const idx = _perl_range(0, n).filter((j) => cards[j] === s);
        if (idx.length !== 1) {
            throw "Foo";
        }
        const j = idx[0];
        ints.push(j);
        cards[j] = cards[n];
        --n;
    }

    return ints;
}
