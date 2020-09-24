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

export function get_flipped_deal_with_comment_prefix(): string {
    return flipped_deal_with_comment_prefix;
}
export function get_flipped_deal_with_leading_empty_line(): string {
    return flipped_deal_with_leading_empty_line;
}
