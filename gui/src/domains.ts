(function () {
	const colors = ["#35a962", "#b58716", "#fb3e3c", "#f41cc8", "#b46df1", "#379be7"];

	type Coord = [number, number];

	const canvas: HTMLCanvasElement = document.getElementById("c") as HTMLCanvasElement;
	canvas.width = window.innerWidth;
	canvas.height = window.innerHeight;
	const ctx = canvas.getContext("2d")!;

	ctx.transform(1, 0, 0, -1, Math.round(canvas.width / 2), Math.round(canvas.height / 2));
	const a = 30;

	function indexToHexCenter([i, j]: Coord): Coord {
		return [i * a * 1.5, j * a * Math.sqrt(3) + (i * a * Math.sqrt(3)) / 2];
	}

	function indexToRhombusCorner([i, j]: Coord): Coord {
		return [i * a - (j * a) / 2, (j * a * Math.sqrt(3)) / 2];
	}

	function d2([x1, y1]: Coord, [x2, y2]: Coord) {
		return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
	}

	const domains = new Map<string, number>();
	const trimers = new Map<string, number>();

	function drawLattice() {
		ctx.beginPath();
		for (let i = -10; i < 10; i++) {
			for (let j = -10; j < 10; j++) {
				let [sx, sy] = indexToHexCenter([i, j]);
				sx -= a / 2;
				sy -= (a * Math.sqrt(3)) / 2;
				ctx.moveTo(sx - a / 2, sy + (a * Math.sqrt(3)) / 2);
				ctx.lineTo(sx, sy);
				ctx.lineTo(sx + a, sy);
				ctx.lineTo(sx + (a * 3) / 2, sy + (a * Math.sqrt(3)) / 2);
			}
		}
		ctx.strokeStyle = "#999999";
		ctx.stroke();

		for (const pair of trimers) {
			const tokens = pair[0].slice(0, pair[0].length - 1).split(".");
			const xy = indexToRhombusCorner([parseInt(tokens[0]), parseInt(tokens[1])]);

			ctx.beginPath();
			ctx.moveTo(xy[0], xy[1]);
			ctx.lineTo(xy[0] + a / 2, xy[1] + (a * Math.sqrt(3)) / 2);
			if (pair[0][pair[0].length - 1] === "+") {
				ctx.lineTo(xy[0] - a / 2, xy[1] + (a * Math.sqrt(3)) / 2);
			} else {
				ctx.lineTo(xy[0] + a, xy[1]);
			}
			ctx.closePath();
			ctx.fillStyle = colors[pair[1] - 1];
			ctx.fill();
		}
	}

	function locateMouseClick(e: MouseEvent) {
		const x = e.clientX - canvas.width / 2;
		const y = -e.clientY + canvas.height / 2;

		const left = Math.floor(x / (a * 1.5));
		const bottom = Math.floor(y / (a * Math.sqrt(3)) - x / (a * 1.5) / 2);

		const candidates: Coord[] = [
			[left, bottom],
			[left + 1, bottom],
			[left, bottom + 1],
			[left + 1, bottom + 1],
		];

		let best = Infinity;
		let besti = -1;

		for (let i = 0; i < candidates.length; i++) {
			const cand = indexToHexCenter(candidates[i]);
			const d = d2(indexToHexCenter(candidates[i]), [x, y]);
			if (d < best) {
				best = d;
				besti = i;
			}
		}

		return candidates[besti];
	}

	function setDomain(index: Coord, domain?: number) {
		if (domain !== undefined) {
			domains.set(`${index[0]}.${index[1]}`, domain);
		} else {
			domains.delete(`${index[0]}.${index[1]}`);
		}

		const base = [index[0] * 2 + index[1] - 1, index[0] + index[1] * 2 - 1];
		const tris = [
			`${base[0] + 1}.${base[1] + 1}+`,
			`${base[0] + 1}.${base[1] + 1}-`,
			`${base[0] + 1}.${base[1]}+`,
			`${base[0]}.${base[1]}-`,
			`${base[0]}.${base[1]}+`,
			`${base[0]}.${base[1] + 1}-`,
		];
		for (let i = 0; i < 6; i++) {
			if (domain !== undefined && i === domain - 1) {
				trimers.set(tris[i], domain);
			} else {
				trimers.delete(tris[i]);
			}
		}
	}

	drawLattice();
	let mousepos: MouseEvent;
	canvas.onmousemove = e => {
		mousepos = e;
	};
	window.onkeydown = e => {
		const num = e.key.charCodeAt(0) - "0".charCodeAt(0);
		if (num >= 1 && num <= 6) {
			setDomain(locateMouseClick(mousepos), num);
		}
		if (num === 0) {
			setDomain(locateMouseClick(mousepos));
		}
		ctx.clearRect(-canvas.width, -canvas.height, canvas.width * 2, canvas.height * 2);
		drawLattice();
	};
})();
