'use strict';

const WORKER_BEES = 5;
const OBSERVER_BEES = 55;
const VERTICES_COUNT = 300;

const probability = 0.05;
let MaxColor = 0;
const AllColors = [];
const UsedColors = [];

const printMatrix = (matrix: number[][]) => {
  matrix.map(arr => console.log(JSON.stringify(arr)));
};

const graphGen = (n : number) : number[][] => {
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

const checkGraph = (matrix: number[][]) : boolean => {
  let res = true;
  matrix.map(row => {
    const count: number = row.filter(n => n === 1).length;
    if (count < 1 || count > 30) res = false;
  });
  return res;
};

class WorkerBee {
  constructor(private matrix: number[][]) {}

  inspect(vertex: number) : number {
    const power: number = this.matrix[vertex].filter(n => n === 1).length;
    return power;
  }
}

class ObserverBee {
  constructor(private matrix: number[][]) {}

  color(vertex: number) : void {

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

for (let i = 0; i < OBSERVER_BEES; i++)
  observers.push(new ObserverBee(m));

// Iteration

const unvisited = [];
const nectar = new Map();
let totalNectar = 0;

for (let i = 0; i < VERTICES_COUNT; i++)
  unvisited.push(i);

workers.forEach(worker => {
  const vertex = Math.floor(Math.random() * unvisited.length);
  unvisited.splice(unvisited.indexOf(vertex), 1);
  const value = worker.inspect(vertex);
  totalNectar += value;
  nectar.set(vertex, value);
});

console.log(totalNectar);
console.log(nectar);
console.log(unvisited.length);