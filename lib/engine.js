// Priority engine — mirrors PRD Section 7 (Core Patient Priority Engine).
// Pure, framework-free functions so the same logic can later be ported
// 1:1 into the C++ PriorityEngine / ScoreCalculator classes.

export const DEFAULT_CONFIG = {
  symptomMax: 30,
  vitalMax: 25,
  painMax: 15,
  ageMax: 10,
  riskMax: 10,
  waitInterval: 10, // minutes per waiting-time point
  waitCap: 10, // max bonus points
};

export function levelOf(score) {
  if (score >= 80) return 'critical';
  if (score >= 60) return 'high';
  if (score >= 40) return 'medium';
  return 'low';
}

export function levelLabel(level) {
  return { critical: 'Critical', high: 'High', medium: 'Medium', low: 'Low' }[level];
}

export function waitingBonus(waitMinutes, config) {
  return Math.min(Math.floor(waitMinutes / config.waitInterval), config.waitCap);
}

export function effectiveScore(patient, config) {
  return Math.min(patient.urgencyScore + waitingBonus(patient.waitMinutes, config), 100);
}

export function sortedByPriority(list, config) {
  return [...list].sort(
    (a, b) => effectiveScore(b, config) - effectiveScore(a, config) || a.arrival - b.arrival
  );
}

export function sortedByArrival(list) {
  return [...list].sort((a, b) => a.arrival - b.arrival);
}

export function priorityInversions(waiting, config) {
  const arr = sortedByArrival(waiting);
  let count = 0;
  for (let i = 0; i < arr.length; i++) {
    for (let j = i + 1; j < arr.length; j++) {
      if (effectiveScore(arr[j], config) > effectiveScore(arr[i], config)) count++;
    }
  }
  return count;
}

export function fmtWait(min) {
  if (min < 60) return `${min} min`;
  return `${Math.floor(min / 60)}h ${min % 60}m`;
}

export function fmtTime(d) {
  return d.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' });
}

export function initials(name) {
  return name.split(' ').map((w) => w[0]).slice(0, 2).join('');
}
