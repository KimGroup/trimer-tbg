"use strict";
var __generator = (this && this.__generator) || function (thisArg, body) {
    var _ = { label: 0, sent: function() { if (t[0] & 1) throw t[1]; return t[1]; }, trys: [], ops: [] }, f, y, t, g;
    return g = { next: verb(0), "throw": verb(1), "return": verb(2) }, typeof Symbol === "function" && (g[Symbol.iterator] = function() { return this; }), g;
    function verb(n) { return function (v) { return step([n, v]); }; }
    function step(op) {
        if (f) throw new TypeError("Generator is already executing.");
        while (_) try {
            if (f = 1, y && (t = op[0] & 2 ? y["return"] : op[0] ? y["throw"] || ((t = y["return"]) && t.call(y), 0) : y.next) && !(t = t.call(y, op[1])).done) return t;
            if (y = 0, t) op = [op[0] & 2, t.value];
            switch (op[0]) {
                case 0: case 1: t = op; break;
                case 4: _.label++; return { value: op[1], done: false };
                case 5: _.label++; y = op[1]; op = [0]; continue;
                case 7: op = _.ops.pop(); _.trys.pop(); continue;
                default:
                    if (!(t = _.trys, t = t.length > 0 && t[t.length - 1]) && (op[0] === 6 || op[0] === 2)) { _ = 0; continue; }
                    if (op[0] === 3 && (!t || (op[1] > t[0] && op[1] < t[3]))) { _.label = op[1]; break; }
                    if (op[0] === 6 && _.label < t[1]) { _.label = t[1]; t = op; break; }
                    if (t && _.label < t[2]) { _.label = t[2]; _.ops.push(op); break; }
                    if (t[2]) _.ops.pop();
                    _.trys.pop(); continue;
            }
            op = body.call(thisArg, _);
        } catch (e) { op = [6, e]; y = 0; } finally { f = t = 0; }
        if (op[0] & 5) throw op[1]; return { value: op[0] ? op[1] : void 0, done: true };
    }
};
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
    var canvas = document.getElementById("c");
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;
    var ctx = canvas.getContext("2d");
    var a = 30;
    var b = Math.sqrt(3) / 2;
    function toOrtho(x) {
        // return [x[0] / a, (canvas.height - x[1]) / a];
        return [(x[0] - (canvas.height - x[1]) / b / 2) / a, (canvas.height - x[1]) / b / a];
    }
    function fromOrtho(x) {
        // return [x[0] * a, canvas.height - x[1] * a];
        return [(x[0] + x[1] / 2) * a, canvas.height - x[1] * a * b];
    }
    function plus(x, y) {
        return [x[0] + y[0], x[1] + y[1]];
    }
    function copy(x) {
        return [x[0], x[1]];
    }
    function getDotsOfTrimer(x) {
        if (!x.upper) {
            return [copy(x.coord), plus(x.coord, [1, 0]), plus(x.coord, [0, 1])];
        }
        else {
            return [plus(x.coord, [1, 1]), plus(x.coord, [1, 0]), plus(x.coord, [0, 1])];
        }
    }
    function serializeCoord(x) {
        return x[0] + "." + x[1];
    }
    function serializeTrimer(x) {
        return serializeCoord(x.coord) + "." + (x.upper ? "U" : "D");
    }
    var TOP = 0;
    var PARENT = function (i) { return ((i + 1) >>> 1) - 1; };
    var LEFT = function (i) { return (i << 1) + 1; };
    var RIGHT = function (i) { return (i + 1) << 1; };
    var PriorityQueue = /** @class */ (function () {
        function PriorityQueue(comparator) {
            if (comparator === void 0) { comparator = function (a, b) { return a > b; }; }
            this._heap = [];
            this._comparator = comparator;
        }
        PriorityQueue.prototype.copy = function () {
            var ret = new PriorityQueue(this._comparator);
            ret._heap = this._heap.slice();
            return ret;
        };
        PriorityQueue.prototype.size = function () {
            return this._heap.length;
        };
        PriorityQueue.prototype.isEmpty = function () {
            return this.size() === 0;
        };
        PriorityQueue.prototype.peek = function () {
            return this._heap[TOP];
        };
        PriorityQueue.prototype.push = function () {
            var _this = this;
            var values = [];
            for (var _i = 0; _i < arguments.length; _i++) {
                values[_i] = arguments[_i];
            }
            values.forEach(function (value) {
                _this._heap.push(value);
                _this._siftUp();
            });
            return this.size();
        };
        PriorityQueue.prototype.pop = function () {
            var poppedValue = this.peek();
            var bottom = this.size() - 1;
            if (bottom > TOP) {
                this._swap(TOP, bottom);
            }
            this._heap.pop();
            this._siftDown();
            return poppedValue;
        };
        PriorityQueue.prototype.replace = function (value) {
            var replacedValue = this.peek();
            this._heap[TOP] = value;
            this._siftDown();
            return replacedValue;
        };
        PriorityQueue.prototype._greater = function (i, j) {
            return this._comparator(this._heap[i], this._heap[j]);
        };
        PriorityQueue.prototype._swap = function (i, j) {
            var _a;
            _a = __read([this._heap[j], this._heap[i]], 2), this._heap[i] = _a[0], this._heap[j] = _a[1];
        };
        PriorityQueue.prototype._siftUp = function () {
            var node = this.size() - 1;
            while (node > TOP && this._greater(node, PARENT(node))) {
                this._swap(node, PARENT(node));
                node = PARENT(node);
            }
        };
        PriorityQueue.prototype._siftDown = function () {
            var node = TOP;
            while ((LEFT(node) < this.size() && this._greater(LEFT(node), node)) ||
                (RIGHT(node) < this.size() && this._greater(RIGHT(node), node))) {
                var maxChild = RIGHT(node) < this.size() && this._greater(RIGHT(node), LEFT(node)) ? RIGHT(node) : LEFT(node);
                this._swap(node, maxChild);
                node = maxChild;
            }
        };
        return PriorityQueue;
    }());
    var DotsToTrimers = {};
    var Trimers = {};
    var NearestTrimers = new PriorityQueue(function (a, b) { return Math.pow((a.coord[0] - 20), 2) + Math.pow((a.coord[1] - 20), 2) < Math.pow((b.coord[0] - 20), 2) + Math.pow((b.coord[1] - 20), 2); });
    function getAdjDots(x) {
        if (!x.upper) {
            return [
                [0, -1],
                [1, -1],
                [2, -1],
                [-1, 0],
                [-1, 1],
                [-1, 2],
                [0, 2],
                [1, 1],
                [2, 0],
            ].map(function (y) { return plus(y, x.coord); });
        }
        else {
            return [
                [-1, 2],
                [0, 2],
                [1, 2],
                [2, -1],
                [2, 0],
                [2, 1],
                [-1, 1],
                [0, 0],
                [1, -1],
            ].map(function (y) { return plus(y, x.coord); });
        }
    }
    function getTrimersPointingAt(x) {
        if (!x.upper) {
            return [
                { coord: plus(x.coord, [1, 1]), upper: false },
                { coord: plus(x.coord, [1, -2]), upper: false },
                { coord: plus(x.coord, [-2, 1]), upper: false },
            ];
        }
        else {
            return [
                { coord: plus(x.coord, [-1, -1]), upper: true },
                { coord: plus(x.coord, [2, -1]), upper: true },
                { coord: plus(x.coord, [-1, 2]), upper: true },
            ];
        }
    }
    function getTrimersPointedAtBy(x) {
        if (!x.upper) {
            return [
                { coord: plus(x.coord, [-1, -1]), upper: false },
                { coord: plus(x.coord, [2, -1]), upper: false },
                { coord: plus(x.coord, [-1, 2]), upper: false },
            ];
        }
        else {
            return [
                { coord: plus(x.coord, [1, 1]), upper: true },
                { coord: plus(x.coord, [1, -2]), upper: true },
                { coord: plus(x.coord, [-2, 1]), upper: true },
            ];
        }
    }
    function getExchangeTrimers(x) {
        if (!x.upper) {
            return [
                { coord: plus(x.coord, [-2, 0]), upper: true },
                { coord: plus(x.coord, [-2, 1]), upper: true },
                { coord: plus(x.coord, [0, -2]), upper: true },
                { coord: plus(x.coord, [1, -2]), upper: true },
                { coord: plus(x.coord, [1, 0]), upper: true },
                { coord: plus(x.coord, [0, 1]), upper: true },
            ];
        }
        else {
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
    function getSurroundingTrimers(x) {
        return [
            { coord: plus(x, [0, -1]), upper: true },
            { coord: plus(x, [0, -1]), upper: false },
            { coord: plus(x, [-1, 0]), upper: true },
            { coord: plus(x, [-1, 0]), upper: false },
            { coord: plus(x, [-1, -1]), upper: true },
            { coord: plus(x, [0, 0]), upper: false },
        ];
    }
    function iterateAdjDots() {
        var seen, copy, v, _a, _b, x, ser, e_1_1;
        var e_1, _c;
        return __generator(this, function (_d) {
            switch (_d.label) {
                case 0:
                    seen = new Set();
                    copy = NearestTrimers.copy();
                    _d.label = 1;
                case 1:
                    if (!(copy.size() > 0)) return [3 /*break*/, 10];
                    v = copy.pop();
                    _d.label = 2;
                case 2:
                    _d.trys.push([2, 7, 8, 9]);
                    _a = (e_1 = void 0, __values(getAdjDots(v))), _b = _a.next();
                    _d.label = 3;
                case 3:
                    if (!!_b.done) return [3 /*break*/, 6];
                    x = _b.value;
                    ser = serializeCoord(x);
                    if (!(!DotsToTrimers[ser] && !seen.has(ser) && Math.abs(x[0]) + Math.abs(x[1]) < 100)) return [3 /*break*/, 5];
                    return [4 /*yield*/, x];
                case 4:
                    _d.sent();
                    seen.add(ser);
                    _d.label = 5;
                case 5:
                    _b = _a.next();
                    return [3 /*break*/, 3];
                case 6: return [3 /*break*/, 9];
                case 7:
                    e_1_1 = _d.sent();
                    e_1 = { error: e_1_1 };
                    return [3 /*break*/, 9];
                case 8:
                    try {
                        if (_b && !_b.done && (_c = _a.return)) _c.call(_a);
                    }
                    finally { if (e_1) throw e_1.error; }
                    return [7 /*endfinally*/];
                case 9: return [3 /*break*/, 1];
                case 10: return [2 /*return*/];
            }
        });
    }
    function getTrimerCenter(trimer) {
        var pts = getDotsOfTrimer(trimer);
        return [(pts[0][0] + pts[1][0] + pts[2][0]) / 3, (pts[0][1] + pts[1][1] + pts[2][1]) / 3];
    }
    function drawTrimer(trimer) {
        var tempStyle = ctx.fillStyle;
        var pts = getDotsOfTrimer(trimer);
        ctx.fillStyle = "white";
        pts.forEach(function (x) {
            var y = fromOrtho(x);
            ctx.fillRect(y[0] - 2, y[1] - 2, 5, 5);
        });
        ctx.fillStyle = tempStyle;
        ctx.beginPath();
        var p = fromOrtho(pts[0]);
        ctx.moveTo(p[0], p[1]);
        p = fromOrtho(pts[1]);
        ctx.lineTo(p[0], p[1]);
        p = fromOrtho(pts[2]);
        ctx.lineTo(p[0], p[1]);
        ctx.closePath();
        ctx.fill();
        ctx.strokeStyle = "blue";
        var center = fromOrtho(getTrimerCenter(trimer));
        getExchangeTrimers(trimer)
            .filter(function (x) { return Trimers[serializeTrimer(x)]; })
            .forEach(function (x) {
            ctx.beginPath();
            ctx.moveTo(center[0], center[1]);
            var newcenter = fromOrtho(getTrimerCenter(x));
            ctx.lineTo(newcenter[0], newcenter[1]);
            ctx.stroke();
        });
    }
    function doesTrimerOverlap(x) {
        return getDotsOfTrimer(x).some(function (z) { return DotsToTrimers[serializeCoord(z)]; });
    }
    function commitTrimer(tri) {
        Trimers[serializeTrimer(tri)] = tri;
        NearestTrimers.push(tri);
        getDotsOfTrimer(tri).forEach(function (x) { return (DotsToTrimers[serializeCoord(x)] = tri); });
    }
    canvas.onclick = function (e) {
        var orth = toOrtho([e.x, e.y]);
        var orthP = [Math.floor(orth[0]), Math.floor(orth[1])];
        var trimer = { upper: orth[0] + orth[1] - orthP[0] - orthP[1] > 1, coord: copy(orthP) };
        commitTrimer(trimer);
        ctx.fillStyle = "pink";
        drawTrimer(trimer);
        /*
    getTrimersPointedAtBy(trimer).forEach(drawTrimer);
    getTrimersPointingAt(trimer).forEach(drawTrimer);
    getExchangeTrimers(trimer).forEach(drawTrimer);
    */
        var work = function () {
            if (workIteration())
                setTimeout(work, 20);
        };
        work();
    };
    function drawGrid() {
        ctx.fillStyle = "white";
        ctx.fillRect(0, 0, canvas.width, canvas.height);
        ctx.fillStyle = "black";
        for (var y = 0; y < 40; y++) {
            for (var x = -Math.round(y / 2); x < 50; x++) {
                var pos = [x, y];
                pos = fromOrtho(pos);
                ctx.fillRect(pos[0], pos[1], 2, 2);
            }
        }
    }
    drawGrid();
    function workIteration() {
        var e_2, _a, e_3, _b;
        ctx.fillStyle = "lightblue";
        try {
            for (var _c = __values(iterateAdjDots()), _d = _c.next(); !_d.done; _d = _c.next()) {
                var x = _d.value;
                var filtered = getSurroundingTrimers(x).filter(function (y) { return !doesTrimerOverlap(y); });
                if (filtered.length === 1) {
                    drawTrimer(filtered[0]);
                    commitTrimer(filtered[0]);
                    return true;
                }
            }
        }
        catch (e_2_1) { e_2 = { error: e_2_1 }; }
        finally {
            try {
                if (_d && !_d.done && (_a = _c.return)) _a.call(_c);
            }
            finally { if (e_2) throw e_2.error; }
        }
        try {
            for (var _e = __values(iterateAdjDots()), _f = _e.next(); !_f.done; _f = _e.next()) {
                var x = _f.value;
                var filtered = getSurroundingTrimers(x).filter(function (y) {
                    // disallow charge overlap
                    return !doesTrimerOverlap(y) &&
                        // disallow trimers pointed at
                        !getTrimersPointingAt(y).some(function (z) { return Trimers[serializeTrimer(z)]; }) &&
                        // disallow pointing at other trimers
                        !getTrimersPointedAtBy(y).some(function (z) { return Trimers[serializeTrimer(z)]; });
                });
                if (filtered.length === 1) {
                    var tri = filtered[0];
                    drawTrimer(tri);
                    commitTrimer(tri);
                    return true;
                }
            }
        }
        catch (e_3_1) { e_3 = { error: e_3_1 }; }
        finally {
            try {
                if (_f && !_f.done && (_b = _e.return)) _b.call(_e);
            }
            finally { if (e_3) throw e_3.error; }
        }
        return false;
    }
})();
