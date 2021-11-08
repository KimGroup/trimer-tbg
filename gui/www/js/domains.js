"use strict";
var __read = (this && this.__read) || function (o, n) {
    var m = typeof Symbol === "function" && o[Symbol.iterator];
    if (!m) return o;
    var i = m.call(o), r, ar = [], e;
    try {
        while ((n === void 0 || n-- > 0) && !(r = i.next()).done) ar.push(r.value);
    }
    catch (error) { e = { error: error }; }
    finally {
        try {
            if (r && !r.done && (m = i["return"])) m.call(i);
        }
        finally { if (e) throw e.error; }
    }
    return ar;
};
var __values = (this && this.__values) || function(o) {
    var s = typeof Symbol === "function" && Symbol.iterator, m = s && o[s], i = 0;
    if (m) return m.call(o);
    if (o && typeof o.length === "number") return {
        next: function () {
            if (o && i >= o.length) o = void 0;
            return { value: o && o[i++], done: !o };
        }
    };
    throw new TypeError(s ? "Object is not iterable." : "Symbol.iterator is not defined.");
};
(function () {
    var colors = ["#35a962", "#b58716", "#fb3e3c", "#f41cc8", "#b46df1", "#379be7"];
    var canvas = document.getElementById("c");
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;
    var ctx = canvas.getContext("2d");
    ctx.transform(1, 0, 0, -1, Math.round(canvas.width / 2), Math.round(canvas.height / 2));
    var a = 30;
    function indexToHexCenter(_a) {
        var _b = __read(_a, 2), i = _b[0], j = _b[1];
        return [i * a * 1.5, j * a * Math.sqrt(3) + (i * a * Math.sqrt(3)) / 2];
    }
    function indexToRhombusCorner(_a) {
        var _b = __read(_a, 2), i = _b[0], j = _b[1];
        return [i * a - (j * a) / 2, (j * a * Math.sqrt(3)) / 2];
    }
    function d2(_a, _b) {
        var _c = __read(_a, 2), x1 = _c[0], y1 = _c[1];
        var _d = __read(_b, 2), x2 = _d[0], y2 = _d[1];
        return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
    }
    var domains = new Map();
    var trimers = new Map();
    function drawLattice() {
        var e_1, _a;
        ctx.beginPath();
        for (var i = -10; i < 10; i++) {
            for (var j = -10; j < 10; j++) {
                var _b = __read(indexToHexCenter([i, j]), 2), sx = _b[0], sy = _b[1];
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
        try {
            for (var trimers_1 = __values(trimers), trimers_1_1 = trimers_1.next(); !trimers_1_1.done; trimers_1_1 = trimers_1.next()) {
                var pair = trimers_1_1.value;
                var tokens = pair[0].slice(0, pair[0].length - 1).split(".");
                var xy = indexToRhombusCorner([parseInt(tokens[0]), parseInt(tokens[1])]);
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
        }
        catch (e_1_1) { e_1 = { error: e_1_1 }; }
        finally {
            try {
                if (trimers_1_1 && !trimers_1_1.done && (_a = trimers_1.return)) _a.call(trimers_1);
            }
            finally { if (e_1) throw e_1.error; }
        }
    }
    function locateMouseClick(e) {
        var x = e.clientX - canvas.width / 2;
        var y = -e.clientY + canvas.height / 2;
        var left = Math.floor(x / (a * 1.5));
        var bottom = Math.floor(y / (a * Math.sqrt(3)) - x / (a * 1.5) / 2);
        var candidates = [
            [left, bottom],
            [left + 1, bottom],
            [left, bottom + 1],
            [left + 1, bottom + 1],
        ];
        var best = Infinity;
        var besti = -1;
        for (var i = 0; i < candidates.length; i++) {
            var cand = indexToHexCenter(candidates[i]);
            var d = d2(indexToHexCenter(candidates[i]), [x, y]);
            if (d < best) {
                best = d;
                besti = i;
            }
        }
        return candidates[besti];
    }
    function setDomain(index, domain) {
        if (domain !== undefined) {
            domains.set(index[0] + "." + index[1], domain);
        }
        else {
            domains.delete(index[0] + "." + index[1]);
        }
        var base = [index[0] * 2 + index[1] - 1, index[0] + index[1] * 2 - 1];
        var tris = [
            base[0] + 1 + "." + (base[1] + 1) + "+",
            base[0] + 1 + "." + (base[1] + 1) + "-",
            base[0] + 1 + "." + base[1] + "+",
            base[0] + "." + base[1] + "-",
            base[0] + "." + base[1] + "+",
            base[0] + "." + (base[1] + 1) + "-",
        ];
        for (var i = 0; i < 6; i++) {
            if (domain !== undefined && i === domain - 1) {
                trimers.set(tris[i], domain);
            }
            else {
                trimers.delete(tris[i]);
            }
        }
    }
    drawLattice();
    var mousepos;
    canvas.onmousemove = function (e) {
        mousepos = e;
    };
    window.onkeydown = function (e) {
        var num = e.key.charCodeAt(0) - "0".charCodeAt(0);
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
