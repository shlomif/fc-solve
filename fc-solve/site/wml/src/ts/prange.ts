export function perl_range(start: number, end: number): number[] {
    const ret: number[] = [];

    for (let i = start; i <= end; ++i) {
        ret.push(i);
    }

    return ret;
}
