"use strict";
(function () {
    const colors = ["#4dab2f", "#decd2b", "#f78026", "#fd58f7", "#9c78fa", "#4db2f1"];
    const canvas = document.getElementById("c");
    // canvas.width = window.innerWidth;
    // canvas.height = window.innerHeight;
    canvas.width = 400;
    canvas.height = 250;
    const ctx = canvas.getContext("2d");
    ctx.transform(1, 0, 0, -1, Math.round(canvas.width / 2), Math.round(canvas.height / 2));
    const a = 40;
    function indexToHexCenter([i, j]) {
        return [i * a * 1.5, j * a * Math.sqrt(3) + (i * a * Math.sqrt(3)) / 2];
    }
    function indexToRhombusCorner([i, j]) {
        return [i * a - (j * a) / 2, (j * a * Math.sqrt(3)) / 2];
    }
    function d2([x1, y1], [x2, y2]) {
        return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
    }
    const domains = new Map();
    const trimers = new Map();
    function drawLattice() {
        for (const pair of trimers) {
            const tokens = pair[0].slice(0, pair[0].length - 1).split(".");
            const xy = indexToRhombusCorner([parseInt(tokens[0]), parseInt(tokens[1])]);
            ctx.beginPath();
            ctx.moveTo(xy[0], xy[1]);
            ctx.lineTo(xy[0] + a / 2, xy[1] + (a * Math.sqrt(3)) / 2);
            if (pair[0][pair[0].length - 1] === "+") {
                ctx.lineTo(xy[0] - a / 2, xy[1] + (a * Math.sqrt(3)) / 2);
            }
            else {
                ctx.lineTo(xy[0] + a, xy[1]);
            }
            ctx.closePath();
            ctx.fillStyle = colors[pair[1] - 1];
            ctx.fill();
        }
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
                for (let k = 0; k < 6; k++) {
                    ctx.moveTo(sx + a / 2, sy + (a * Math.sqrt(3)) / 2);
                    ctx.lineTo(sx + a / 2 + a * Math.cos((k * Math.PI) / 3), sy + (a * Math.sqrt(3)) / 2 + a * Math.sin((k * Math.PI) / 3));
                }
            }
        }
        ctx.strokeStyle = "#0000bb";
        ctx.stroke();
        ctx.beginPath();
        for (let i = -10; i < 10; i++) {
            for (let j = -10; j < 10; j++) {
                let [sx, sy] = indexToHexCenter([i, j]);
                sx -= a / 2;
                sy -= (a * Math.sqrt(3)) / 2;
                const innerd = a * (Math.sqrt(3) / 2 - 1 / Math.sqrt(3) / 2);
                for (let k = 0; k < 6; k++) {
                    let [x, y] = [
                        sx + a / 2 + innerd * Math.cos((k * Math.PI) / 3 + Math.PI / 6),
                        sy + (a * Math.sqrt(3)) / 2 + innerd * Math.sin((k * Math.PI) / 3 + Math.PI / 6),
                    ];
                    if (k == 0)
                        ctx.moveTo(x, y);
                    else
                        ctx.lineTo(x, y);
                }
                ctx.closePath();
                for (let k = 0; k < 3; k++) {
                    let [x, y] = [
                        sx + a / 2 + innerd * Math.cos((k * Math.PI) / 3 + Math.PI / 6),
                        sy + (a * Math.sqrt(3)) / 2 + innerd * Math.sin((k * Math.PI) / 3 + Math.PI / 6),
                    ];
                    ctx.moveTo(x, y);
                    ctx.lineTo(x + (a / Math.sqrt(3)) * Math.cos((k * Math.PI) / 3 + Math.PI / 6), y + (a / Math.sqrt(3)) * Math.sin((k * Math.PI) / 3 + Math.PI / 6));
                }
            }
        }
        ctx.strokeStyle = "#bb0000";
        ctx.stroke();
    }
    function locateMouseClick(e) {
        const x = e.clientX - canvas.width / 2;
        const y = -e.clientY + canvas.height / 2;
        const left = Math.floor(x / (a * 1.5));
        const bottom = Math.floor(y / (a * Math.sqrt(3)) - x / (a * 1.5) / 2);
        const candidates = [
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
    function setDomain(index, domain) {
        if (domain !== undefined) {
            domains.set(`${index[0]}.${index[1]}`, domain);
        }
        else {
            domains.delete(`${index[0]}.${index[1]}`);
        }
        const base = [index[0] * 2 + index[1] - 1, index[0] + index[1] * 2 - 1];
        const tris = [
            `${base[0]}.${base[1] + 1}-`,
            `${base[0] + 1}.${base[1] + 1}+`,
            `${base[0] + 1}.${base[1] + 1}-`,
            `${base[0] + 1}.${base[1]}+`,
            `${base[0]}.${base[1]}-`,
            `${base[0]}.${base[1]}+`,
        ];
        for (let i = 0; i < 6; i++) {
            if (domain !== undefined && i === domain - 1) {
                trimers.set(tris[i], domain);
            }
            else {
                trimers.delete(tris[i]);
            }
        }
    }
    drawLattice();
    let mousepos;
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
