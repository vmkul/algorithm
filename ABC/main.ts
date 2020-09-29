'use strict';

const WORKER_BEES = 5;
const OBSERVER_BEES = 55;
const VERTICES_COUNT = 300;
const ITERATION_COUNT = 1000;

const probability = 0.05;
let UsedColors = [ 0 ];

const printMatrix = (matrix: number[][]) => {
  matrix.map(arr => console.log(JSON.stringify(arr)));
};

const graphGen = (n: number): number[][] => {
  const res: number[][] = [];
  for (let i = 0; i < n; i++)
    res.push([]);

  for (let i = 0; i < n; i++) {
    for (let j = i; j < n; j++) {
      let connected: number = Math.random() <= probability ? 1 : 0;
      if (i === j) connected = 0;
      res[i][j] = connected;
      res[j][i] = connected;
    }
  }
  return res;
};

const checkGraph = (matrix: number[][]): boolean => {
  let res = true;
  matrix.map(row => {
    const count: number = row.filter(n => n === 1).length;
    if (count < 1 || count > 30) res = false;
  });
  return res;
};

class WorkerBee {
  constructor(private matrix: number[][]) { }

  inspect(vertex: number): number[] {
    const row = this.matrix[vertex];
    const vertices = [];
    for (let i = 0; i < row.length; i++) {
      if (row[i] === 1)
        vertices.push(i);
    }
    return vertices;
  }
}

class ObserverBee {
  constructor(private matrix: number[][]) { }

  color(vertex: number, colors: Map<number, number>): void {
    const colorNumber = Math.floor(Math.random() * UsedColors.length);
    let color = UsedColors[colorNumber];
    const row = this.matrix[vertex];

    let reserved = 0;
    for (let i = 0; i < row.length; i++) {
      if (row[i] === 1 && colors.get(i) === color) {
        if (reserved === UsedColors.length) {
          color = Math.floor(Math.random() * 1000);
          UsedColors.push(color);
        }
        color = UsedColors[reserved++];
        i = 0;
      }
    }

    colors.set(vertex, color);
  }
}

// Prepare

const m = graphGen(VERTICES_COUNT);
const check = checkGraph(m);

if (!check) {
  console.error('Graph check failed!');
  process.exit(1);
}

const workers: WorkerBee[] = [];
const observers: ObserverBee[] = [];

for (let i = 0; i < WORKER_BEES; i++)
  workers.push(new WorkerBee(m));

// Iteration

const colors = new Map();
let bestChromatic = Infinity;

for (let iter = 0; iter < ITERATION_COUNT; iter++) {
  const unvisited = [];
  const nectar = new Map();

  for (let i = 0; i < VERTICES_COUNT; i++)
    unvisited.push(i);

  while (unvisited.length !== 0) {
    for (let i = 0; i < OBSERVER_BEES; i++)
      observers.push(new ObserverBee(m));

    workers.forEach(worker => {
      const vertex = Math.floor(Math.random() * unvisited.length);
      unvisited.splice(unvisited.indexOf(vertex), 1);
      const value = worker.inspect(vertex);
      nectar.set(vertex, value);
    });

    let totalNectar = 0;
    for (const n of nectar.values()) 
      totalNectar += n.length;

    for (const v of nectar.keys()) {
      observers.pop().color(v, colors);
    }

    for (const nectarVal of nectar.values()) {
      const pi = nectarVal.length / totalNectar;
      let count = pi * (OBSERVER_BEES - WORKER_BEES) - 1;
      nectarVal.forEach(v => {
        if (count-- > 0)
          observers.pop().color(v, colors);
      });
    }

    nectar.clear();
  }

  const uniqueColors = new Set(colors.values());
  UsedColors = Array.from(uniqueColors);
  bestChromatic = Math.min(UsedColors.length, bestChromatic);

  if (iter % 20 === 0) {
    console.log(`Chromatic number: ${bestChromatic} (${UsedColors.length})`);
  }
}

