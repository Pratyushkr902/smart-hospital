'use client';

import { createContext, useContext, useMemo, useState } from 'react';
import { DEFAULT_CONFIG, fmtTime } from '@/lib/engine';
import { initialQueue, initialAuditLog, makePatient, MY_PATIENT_INDEX } from '@/lib/mockData';

const AppContext = createContext(null);

export function AppProvider({ children }) {
  const [queue, setQueue] = useState(() => initialQueue());
  const [myPatientId] = useState(() => queue[MY_PATIENT_INDEX]?.id);
  const [auditLog, setAuditLog] = useState(() => initialAuditLog());
  const [config, setConfig] = useState(() => ({ ...DEFAULT_CONFIG }));

  function logAction(who, action) {
    setAuditLog((prev) => [{ time: fmtTime(new Date()), who, action }, ...prev].slice(0, 30));
  }

  function checkInPatient(data) {
    const patient = makePatient(
      data.name,
      Number(data.age),
      data.gender,
      data.dept,
      data.doctorId,
      data.inputs,
      0,
      'waiting',
      config
    );
    setQueue((prev) => [patient, ...prev]);
    logAction('Reception · P. Menon', `Checked in ${patient.name} — token ${patient.token}`);
    return patient;
  }

  function callNext(doctorId) {
    setQueue((prev) => {
      const waiting = prev.filter((p) => p.status === 'waiting' && (!doctorId || p.doctorId === doctorId));
      if (!waiting.length) return prev;
      const sorted = [...waiting].sort((a, b) => {
        const scoreA = Math.min(a.urgencyScore + Math.min(Math.floor(a.waitMinutes / config.waitInterval), config.waitCap), 100);
        const scoreB = Math.min(b.urgencyScore + Math.min(Math.floor(b.waitMinutes / config.waitInterval), config.waitCap), 100);
        return scoreB - scoreA || a.arrival - b.arrival;
      });
      const top = sorted[0];
      logAction('Dr. A. Sharma', `Called next patient — ${top.name} (${top.token})`);
      return prev.map((p) => (p.id === top.id ? { ...p, status: 'in-treatment', calledAt: new Date() } : p));
    });
  }

  function completeConsult(id, notes) {
    setQueue((prev) => prev.map((p) => (p.id === id ? { ...p, status: 'completed', notes: notes ?? p.notes } : p)));
    const p = queue.find((x) => x.id === id);
    logAction('Dr. A. Sharma', `Completed consultation — ${p ? p.name : ''} (${p ? p.token : ''})`);
  }

  function advanceTime(mins) {
    setQueue((prev) => prev.map((p) => (p.status === 'waiting' ? { ...p, waitMinutes: p.waitMinutes + mins } : p)));
    logAction('System', `Advanced simulation clock by ${mins} minutes`);
  }

  function updateConfigKey(key, value) {
    setConfig((prev) => ({ ...prev, [key]: value }));
  }

  const value = useMemo(
    () => ({
      queue,
      auditLog,
      config,
      myPatientId,
      checkInPatient,
      callNext,
      completeConsult,
      advanceTime,
      updateConfigKey,
      logAction,
    }),
    // eslint-disable-next-line react-hooks/exhaustive-deps
    [queue, auditLog, config, myPatientId]
  );

  return <AppContext.Provider value={value}>{children}</AppContext.Provider>;
}

export function useApp() {
  const ctx = useContext(AppContext);
  if (!ctx) throw new Error('useApp must be used within AppProvider');
  return ctx;
}
