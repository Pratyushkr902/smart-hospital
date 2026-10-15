'use client';

import { createContext, useContext, useEffect, useMemo, useState } from 'react';
import { DEFAULT_CONFIG, fmtTime } from '@/lib/engine';
import { initialQueue, initialAuditLog, makePatient, MY_PATIENT_INDEX } from '@/lib/mockData';

const BACKEND_URL = process.env.NEXT_PUBLIC_BACKEND_URL || 'http://localhost:8080';

const AppContext = createContext(null);

export function AppProvider({ children }) {
  const [queue, setQueue] = useState(() => initialQueue());
  const [myPatientId] = useState(() => queue[MY_PATIENT_INDEX]?.id);
  const [auditLog, setAuditLog] = useState(() => initialAuditLog());
  const [config, setConfig] = useState(() => ({ ...DEFAULT_CONFIG }));
  const [backendOnline, setBackendOnline] = useState(false);

  // Check C++ Backend Health on mount
  useEffect(() => {
    async function checkBackend() {
      try {
        const res = await fetch(`${BACKEND_URL}/api/health`, { method: 'GET', cache: 'no-store' });
        if (res.ok) {
          const data = await res.json();
          if (data.status === 'online') {
            setBackendOnline(true);
            logAction('System', `Connected to C++ Backend REST Engine (${BACKEND_URL})`);
          }
        }
      } catch {
        setBackendOnline(false);
      }
    }
    checkBackend();
  }, []);

  function logAction(who, action) {
    setAuditLog((prev) => [{ time: fmtTime(new Date()), who, action }, ...prev].slice(0, 30));
  }

  async function checkInPatient(data) {
    const localPatient = makePatient(
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

    // Sync with C++ Backend if online
    if (backendOnline) {
      try {
        const res = await fetch(`${BACKEND_URL}/api/checkin`, {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({
            name: data.name,
            age: Number(data.age),
            gender: data.gender,
            dept: data.dept,
            doctorId: data.doctorId,
            pain: data.inputs?.pain || 5,
            vital: data.inputs?.vital || 15,
            heartRate: 80,
            systolicBp: 120,
            oxygenSaturation: 98,
          }),
        });
        if (res.ok) {
          const result = await res.json();
          if (result.token) {
            localPatient.token = result.token;
            localPatient.urgencyScore = result.urgencyScore ?? localPatient.urgencyScore;
          }
        }
      } catch (err) {
        console.warn('Backend sync failed, using client priority engine:', err);
      }
    }

    setQueue((prev) => [localPatient, ...prev]);
    logAction('Reception · P. Menon', `Checked in ${localPatient.name} — token ${localPatient.token}`);
    return localPatient;
  }

  async function callNext(doctorId) {
    // Notify C++ backend if online
    if (backendOnline) {
      try {
        await fetch(`${BACKEND_URL}/api/doctor/call-next`, { method: 'POST' });
      } catch (err) {
        console.warn('Backend call-next sync notice:', err);
      }
    }

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

  async function advanceTime(mins) {
    if (backendOnline) {
      try {
        await fetch(`${BACKEND_URL}/api/simulation/advance-time`, {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ minutes: mins }),
        });
      } catch (err) {
        console.warn('Backend advance-time notice:', err);
      }
    }
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
      backendOnline,
      checkInPatient,
      callNext,
      completeConsult,
      advanceTime,
      updateConfigKey,
      logAction,
    }),
    // eslint-disable-next-line react-hooks/exhaustive-deps
    [queue, auditLog, config, myPatientId, backendOnline]
  );

  return <AppContext.Provider value={value}>{children}</AppContext.Provider>;
}

export function useApp() {
  const ctx = useContext(AppContext);
  if (!ctx) throw new Error('useApp must be used within AppProvider');
  return ctx;
}
