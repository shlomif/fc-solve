const flipped_deal_base_string = `JH 5D 8S 7S KH TS 9D AH
9C 3D 5C AC JD TC JC 7C
5S 9S KD 9H 7D 4S 2C 6D
KC 2S QC 6C 4C 5H QS 8D
6S 3C 3H QH 8H QD TH TD
2H AD 4D KS 6H JS 2D 7H
AS 8C 3S 4H`;

const flipped_deal_with_comment_prefix =
    "# MS Freecell Deal no. 240 flipped\n" + flipped_deal_base_string + "\n\n";

const flipped_deal_with_leading_empty_line =
    "\n" + flipped_deal_base_string + "\n\n";

const non_flipped = `JH 9C 5S KC 6S 2H AS
5D 3D 9S 2S 3C AD 8C
8S 5C KD QC 3H 4D 3S
7S AC 9H 6C QH KS 4H
KH JD 7D 4C 8H 6H
TS TC 4S 5H QD JS
9D JC 2C QS TH 2D
AH 7C 6D 8D TD 7H`;

export function get_flipped_deal_with_comment_prefix(): string {
    return flipped_deal_with_comment_prefix;
}
export function get_flipped_deal_with_leading_empty_line(): string {
    return flipped_deal_with_leading_empty_line;
}
export function get_non_flipped_deal(): string {
    return non_flipped.replace(/^/gms, ": ") + "\n";
}
