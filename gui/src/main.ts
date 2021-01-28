const canvas: HTMLCanvasElement = document.getElementById("c") as HTMLCanvasElement;
canvas.width = window.innerWidth;
canvas.height = window.innerHeight;

const ctx = canvas.getContext("2d")!;

type Coord = [number, number];
const a = 30;
const b = Math.sqrt(3) / 2;

function toOrtho(x: Coord): Coord {
	// return [x[0] / a, (canvas.height - x[1]) / a];
	return [(x[0] - (canvas.height - x[1]) / b / 2) / a, (canvas.height - x[1]) / b / a];
}
function fromOrtho(x: Coord): Coord {
	// return [x[0] * a, canvas.height - x[1] * a];
	return [(x[0] + x[1] / 2) * a, canvas.height - x[1] * a * b];
}
function plus(x: Coord, y: Coord): Coord {
	return [x[0] + y[0], x[1] + y[1]];
}
function copy(x: Coord): Coord {
	return [x[0], x[1]];
}

function getDotsOfTrimer(x: Trimer) {
	if (!x.upper) {
		return [copy(x.coord), plus(x.coord, [1, 0]), plus(x.coord, [0, 1])];
	} else {
		return [plus(x.coord, [1, 1]), plus(x.coord, [1, 0]), plus(x.coord, [0, 1])];
	}
}

function serializeCoord(x: Coord) {
	return `${x[0]}.${x[1]}`;
}
function serializeTrimer(x: Trimer) {
	return `${serializeCoord(x.coord)}.${x.upper ? "U" : "D"}`;
}

const TOP = 0;
const PARENT = (i: number) => ((i + 1) >>> 1) - 1;
const LEFT = (i: number) => (i << 1) + 1;
const RIGHT = (i: number) => (i + 1) << 1;
class PriorityQueue<T> {
	_heap: T[];
	_comparator: (a: T, b: T) => boolean;
	constructor(comparator = (a: T, b: T) => a > b) {
		this._heap = [];
		this._comparator = comparator;
	}
	copy() {
		const ret = new PriorityQueue(this._comparator);
		ret._heap = this._heap.slice();
		return ret;
	}
	size() {
		return this._heap.length;
	}
	isEmpty() {
		return this.size() === 0;
	}
	peek() {
		return this._heap[TOP];
	}
	push(...values: T[]) {
		values.forEach(value => {
			this._heap.push(value);
			this._siftUp();
		});
		return this.size();
	}
	pop() {
		const poppedValue = this.peek();
		const bottom = this.size() - 1;
		if (bottom > TOP) {
			this._swap(TOP, bottom);
		}
		this._heap.pop();
		this._siftDown();
		return poppedValue;
	}
	replace(value: T) {
		const replacedValue = this.peek();
		this._heap[TOP] = value;
		this._siftDown();
		return replacedValue;
	}
	_greater(i: number, j: number) {
		return this._comparator(this._heap[i], this._heap[j]);
	}
	_swap(i: number, j: number) {
		[this._heap[i], this._heap[j]] = [this._heap[j], this._heap[i]];
	}
	_siftUp() {
		let node = this.size() - 1;
		while (node > TOP && this._greater(node, PARENT(node))) {
			this._swap(node, PARENT(node));
			node = PARENT(node);
		}
	}
	_siftDown() {
		let node = TOP;
		while (
			(LEFT(node) < this.size() && this._greater(LEFT(node), node)) ||
			(RIGHT(node) < this.size() && this._greater(RIGHT(node), node))
		) {
			let maxChild = RIGHT(node) < this.size() && this._greater(RIGHT(node), LEFT(node)) ? RIGHT(node) : LEFT(node);
			this._swap(node, maxChild);
			node = maxChild;
		}
	}
}

type Trimer = { upper: boolean; coord: Coord };
const DotsToTrimers: {
	[k in string]: Trimer;
} = {};
const Trimers: {
	[k in string]: Trimer;
} = {};
const NearestTrimers = new PriorityQueue<Trimer>(
	(a, b) => (a.coord[0] - 20) ** 2 + (a.coord[1] - 20) ** 2 < (b.coord[0] - 20) ** 2 + (b.coord[1] - 20) ** 2,
);

function getAdjDots(x: Trimer): Coord[] {
	if (!x.upper) {
		return ([
			[0, -1],
			[1, -1],
			[2, -1],
			[-1, 0],
			[-1, 1],
			[-1, 2],
			[0, 2],
			[1, 1],
			[2, 0],
		] as Coord[]).map(y => plus(y, x.coord));
	} else {
		return ([
			[-1, 2],
			[0, 2],
			[1, 2],
			[2, -1],
			[2, 0],
			[2, 1],
			[-1, 1],
			[0, 0],
			[1, -1],
		] as Coord[]).map(y => plus(y, x.coord));
	}
}

function getTrimersPointingAt(x: Trimer): Trimer[] {
	if (!x.upper) {
		return [
			{ coord: plus(x.coord, [1, 1]), upper: false },
			{ coord: plus(x.coord, [1, -2]), upper: false },
			{ coord: plus(x.coord, [-2, 1]), upper: false },
		];
	} else {
		return [
			{ coord: plus(x.coord, [-1, -1]), upper: true },
			{ coord: plus(x.coord, [2, -1]), upper: true },
			{ coord: plus(x.coord, [-1, 2]), upper: true },
		];
	}
}

function getTrimersPointedAtBy(x: Trimer): Trimer[] {
	if (!x.upper) {
		return [
			{ coord: plus(x.coord, [-1, -1]), upper: false },
			{ coord: plus(x.coord, [2, -1]), upper: false },
			{ coord: plus(x.coord, [-1, 2]), upper: false },
		];
	} else {
		return [
			{ coord: plus(x.coord, [1, 1]), upper: true },
			{ coord: plus(x.coord, [1, -2]), upper: true },
			{ coord: plus(x.coord, [-2, 1]), upper: true },
		];
	}
}

function getExchangeTrimers(x: Trimer): Trimer[] {
	if (!x.upper) {
		return [
			{ coord: plus(x.coord, [-2, 0]), upper: true },
			{ coord: plus(x.coord, [-2, 1]), upper: true },
			{ coord: plus(x.coord, [0, -2]), upper: true },
			{ coord: plus(x.coord, [1, -2]), upper: true },
			{ coord: plus(x.coord, [1, 0]), upper: true },
			{ coord: plus(x.coord, [0, 1]), upper: true },
		];
	} else {
		return [
			{ coord: plus(x.coord, [2, 0]), upper: false },
			{ coord: plus(x.coord, [2, -1]), upper: false },
			{ coord: plus(x.coord, [0, 2]), upper: false },
			{ coord: plus(x.coord, [-1, 2]), upper: false },
			{ coord: plus(x.coord, [-1, 0]), upper: false },
			{ coord: plus(x.coord, [0, -1]), upper: false },
		];
	}
}

function getSurroundingTrimers(x: Coord): Trimer[] {
	return [
		{ coord: plus(x, [0, -1]), upper: true },
		{ coord: plus(x, [0, -1]), upper: false },
		{ coord: plus(x, [-1, 0]), upper: true },
		{ coord: plus(x, [-1, 0]), upper: false },
		{ coord: plus(x, [-1, -1]), upper: true },
		{ coord: plus(x, [0, 0]), upper: false },
	];
}

function* iterateAdjDots() {
	const seen = new Set<string>();

	const copy = NearestTrimers.copy();
	while (copy.size() > 0) {
		const v = copy.pop();
		for (const x of getAdjDots(v)) {
			const ser = serializeCoord(x);
			if (!DotsToTrimers[ser] && !seen.has(ser) && Math.abs(x[0]) + Math.abs(x[1]) < 100) {
				yield x;
				seen.add(ser);
			}
		}
	}
}

function getTrimerCenter(trimer: Trimer): Coord {
	const pts = getDotsOfTrimer(trimer);
	return [(pts[0][0] + pts[1][0] + pts[2][0]) / 3, (pts[0][1] + pts[1][1] + pts[2][1]) / 3];
}

function drawTrimer(trimer: Trimer) {
	const tempStyle = ctx.fillStyle;
	const pts = getDotsOfTrimer(trimer);
	ctx.fillStyle = "white";
	pts.forEach(x => {
		const y = fromOrtho(x);
		ctx.fillRect(y[0] - 2, y[1] - 2, 5, 5);
	});
	ctx.fillStyle = tempStyle;

	ctx.beginPath();
	let p = fromOrtho(pts[0]);
	ctx.moveTo(p[0], p[1]);
	p = fromOrtho(pts[1]);
	ctx.lineTo(p[0], p[1]);
	p = fromOrtho(pts[2]);
	ctx.lineTo(p[0], p[1]);
	ctx.closePath();
	ctx.fill();

	ctx.strokeStyle = "blue";
	const center = fromOrtho(getTrimerCenter(trimer));
	getExchangeTrimers(trimer)
		.filter(x => Trimers[serializeTrimer(x)])
		.forEach(x => {
			ctx.beginPath();
			ctx.moveTo(center[0], center[1]);
			const newcenter = fromOrtho(getTrimerCenter(x));
			ctx.lineTo(newcenter[0], newcenter[1]);
			ctx.stroke();
		});
}

function doesTrimerOverlap(x: Trimer) {
	return getDotsOfTrimer(x).some(z => DotsToTrimers[serializeCoord(z)]);
}

function commitTrimer(tri: Trimer) {
	Trimers[serializeTrimer(tri)] = tri;
	NearestTrimers.push(tri);
	getDotsOfTrimer(tri).forEach(x => (DotsToTrimers[serializeCoord(x)] = tri));
}

canvas.onclick = e => {
	let orth = toOrtho([e.x, e.y]);
	let orthP: Coord = [Math.floor(orth[0]), Math.floor(orth[1])];

	let trimer: Trimer = { upper: orth[0] + orth[1] - orthP[0] - orthP[1] > 1, coord: copy(orthP) };
	commitTrimer(trimer);

	ctx.fillStyle = "pink";
	drawTrimer(trimer);

	/*
	getTrimersPointedAtBy(trimer).forEach(drawTrimer);
    getTrimersPointingAt(trimer).forEach(drawTrimer);
	getExchangeTrimers(trimer).forEach(drawTrimer);
    */

	const work = () => {
		if (workIteration()) setTimeout(work, 20);
	};

	work();
};

function drawGrid() {
	ctx.fillStyle = "white";
	ctx.fillRect(0, 0, canvas.width, canvas.height);
	ctx.fillStyle = "black";

	for (let y = 0; y < 40; y++) {
		for (let x = -Math.round(y / 2); x < 50; x++) {
			let pos: Coord = [x, y];
			pos = fromOrtho(pos);
			ctx.fillRect(pos[0], pos[1], 2, 2);
		}
	}
}

drawGrid();

function workIteration() {
	ctx.fillStyle = "lightblue";

	for (const x of iterateAdjDots()) {
		const filtered = getSurroundingTrimers(x).filter(y => !doesTrimerOverlap(y));

		if (filtered.length === 1) {
			drawTrimer(filtered[0]);
			commitTrimer(filtered[0]);
			return true;
		}
	}

	for (const x of iterateAdjDots()) {
		const filtered = getSurroundingTrimers(x).filter(
			y =>
				// disallow charge overlap
				!doesTrimerOverlap(y) &&
				// disallow trimers pointed at
				!getTrimersPointingAt(y).some(z => Trimers[serializeTrimer(z)]) &&
				// disallow pointing at other trimers
				!getTrimersPointedAtBy(y).some(z => Trimers[serializeTrimer(z)]),
		);

		if (filtered.length === 1) {
			const tri = filtered[0];

			drawTrimer(tri);
			commitTrimer(tri);
			return true;
		}
	}

	return false;
}
