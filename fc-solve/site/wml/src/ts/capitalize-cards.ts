export class BoardTextLine {
    private newline: string;
    private comment: string;
    private prefix: string;
    private content: string;
    constructor(public line: string) {
        const that = this;
        const m1 = line.match(/^([^\n\r]*)([\n\r]*)$/);
        that.newline = m1[2];
        let l = m1[1];
        if (m1[1].match(/#/)) {
            const m2 = m1[1].match(/^(.*?)(#.*)/);
            that.comment = m2[2];
            l = m2[1];
        } else {
            that.comment = "";
        }
        if (l.match(/:/)) {
            const m3 = l.match(/^([^:]*:)(.*)/);
            that.prefix = m3[1];
            that.content = m3[2];
        } else {
            that.prefix = "";
            that.content = l;
        }
        return;
    }
    public getContent(): string {
        return this.content;
    }
    public capitalize(): string {
        const that = this;
        const ret =
            that.prefix +
            that.getContent().toUpperCase() +
            that.comment +
            that.newline;
        return ret;
    }
}

export function capitalize_cards(board: string): string {
    return board
        .match(/[^\n]*\n?/g)
        .map((l) => {
            return new BoardTextLine(l).capitalize();
        })
        .join("");
}
