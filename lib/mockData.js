import { DEFAULT_CONFIG } from './engine';

export const DOCTORS = [
  { id: 'D-101', name: 'Dr. A. Sharma', dept: 'Emergency Medicine' },
  { id: 'D-104', name: 'Dr. R. Iyer', dept: 'General Medicine' },
  { id: 'D-107', name: 'Dr. N. Fernandes', dept: 'Orthopedics' },
];

export const DEPARTMENTS = [
  'Emergency Medicine',
  'General Medicine',
  'Orthopedics',
  'Pediatrics',
  'Cardiology',
];

export const RISK_FACTORS = [
  { key: 'cardiac', label: 'Cardiac history', pts: 4 },
  { key: 'diabetic', label: 'Diabetic', pts: 2 },
  { key: 'immuno', label: 'Immunocompromised', pts: 4 },
  { key: 'pregnant', label: 'Pregnant', pts: 3 },
  { key: 'respiratory', label: 'Respiratory condition', pts: 3 },
];

let tokenSeq = 1000;
function nextToken() {
  tokenSeq += 1;
  return 'T-' + tokenSeq;
}

export function makePatient(name, age, gender, dept, doctorId, inputs, waitMinutes, status, config = DEFAULT_CONFIG) {
  const risk = inputs.risk || [];
  const riskPts = Math.min(
    risk.reduce((s, k) => s + (RISK_FACTORS.find((r) => r.key === k)?.pts || 0), 0),
    config.riskMax
  );
  const urgencyScore =
    Math.min(inputs.symptom, config.symptomMax) +
    Math.min(inputs.vital, config.vitalMax) +
    Math.min(inputs.pain, config.painMax) +
    Math.min(inputs.age, config.ageMax) +
    riskPts;

  return {
    id: 'PT-' + Math.floor(20000 + Math.random() * 9000),
    token: nextToken(),
    name,
    age,
    gender,
    dept,
    doctorId,
    inputs: { ...inputs, risk },
    urgencyScore,
    waitMinutes,
    status, // 'waiting' | 'in-treatment' | 'completed'
    arrival: new Date(Date.now() - waitMinutes * 60000),
    notes: '',
  };
}

export function initialQueue() {
  return [
    makePatient('Asha Rao', 34, 'F', 'General Medicine', 'D-104', { symptom: 10, vital: 5, pain: 4, age: 2, risk: ['diabetic'] }, 55, 'waiting'),
    makePatient('Imran Qureshi', 61, 'M', 'Cardiology', 'D-101', { symptom: 24, vital: 20, pain: 10, age: 8, risk: ['cardiac'] }, 6, 'waiting'),
    makePatient('Fatima Sheikh', 28, 'F', 'General Medicine', 'D-104', { symptom: 6, vital: 3, pain: 5, age: 1, risk: ['pregnant'] }, 80, 'waiting'),
    makePatient('Vikram Nair', 45, 'M', 'Orthopedics', 'D-107', { symptom: 14, vital: 8, pain: 12, age: 3, risk: [] }, 22, 'waiting'),
    makePatient('Leela Menon', 72, 'F', 'General Medicine', 'D-104', { symptom: 18, vital: 16, pain: 6, age: 9, risk: ['respiratory'] }, 15, 'waiting'),
    makePatient('Sourav Das', 8, 'M', 'Pediatrics', 'D-104', { symptom: 9, vital: 4, pain: 7, age: 6, risk: [] }, 40, 'waiting'),
  ];
}

export function initialAuditLog() {
  return [
    { time: '08:41', who: 'Reception · P. Menon', action: 'Checked in patient 24601 · walk-in' },
    { time: '08:22', who: 'Dr. A. Sharma', action: 'Completed consultation for 24588' },
    { time: '08:05', who: 'System', action: 'Shift queue reset for Emergency Medicine' },
  ];
}

// id of the demo-logged-in patient used on the Patient dashboard
export const MY_PATIENT_INDEX = 2; // Fatima Sheikh

export const ROLE_LABEL = {
  reception: 'Receptionist',
  doctor: 'Doctor',
  admin: 'Administrator',
  patient: 'Patient',
};

export const ROLE_WHO = {
  reception: 'P. Menon',
  doctor: 'Dr. A. Sharma',
  admin: 'S. Kulkarni',
  patient: 'Fatima Sheikh',
};

export const ROLE_HOME = {
  reception: '/reception/checkin',
  doctor: '/doctor/queue',
  admin: '/admin/overview',
  patient: '/patient',
};
