'use client';

import { useState } from 'react';
import { useApp } from '@/context/AppProvider';
import { DOCTORS, RISK_FACTORS } from '@/lib/mockData';
import { effectiveScore, levelOf, sortedByPriority, sortedByArrival, priorityInversions, fmtWait, waitingBonus } from '@/lib/engine';
import { Stat, Pill } from './Bits';
import { Icon } from './Icons';
import { QueueTable, ComparePanel } from './QueueTable';

export default function QueueBody({ isDoctor }) {
  const { queue, config, callNext, completeConsult, advanceTime } = useApp();
  const [compare, setCompare] = useState(false);
  const [activePatientId, setActivePatientId] = useState(null);
  const [notes, setNotes] = useState('');

  const waiting = queue.filter((p) => p.status === 'waiting');
  const smart = sortedByPriority(waiting, config);
  const fifo = sortedByArrival(waiting);
  const inversions = priorityInversions(waiting, config);
  const avgWait = waiting.length ? Math.round(waiting.reduce((s, p) => s + p.waitMinutes, 0) / waiting.length) : 0;
  const criticalCount = waiting.filter((p) => levelOf(effectiveScore(p, config)) === 'critical').length;

  const activePatient = queue.find((p) => p.id === activePatientId);

  function handleConsult(p) {
    setActivePatientId(p.id);
    setNotes(p.notes || '');
  }

  function handleCallNext() {
    const next = smart.find((p) => p.doctorId === 'D-101');
    if (!next) return;
    callNext('D-101');
    setActivePatientId(next.id);
    setNotes(next.notes || '');
  }

  const action = isDoctor
    ? (p) => (
        <button
          onClick={() => handleConsult(p)}
          className="bg-brand hover:bg-brandDark text-white rounded px-2.5 py-1.5 text-xs font-medium"
        >
          Consult
        </button>
      )
    : undefined;

  return (
    <div className="flex flex-col gap-4.5">
      <div className="grid grid-cols-4 gap-3.5">
        <Stat value={waiting.length} label="Waiting now" />
        <Stat value={criticalCount} label="Critical" />
        <Stat value={fmtWait(avgWait)} label="Average wait" />
        <Stat value={inversions} label="Inversions vs. arrival order" />
      </div>

      <div className="border border-border rounded bg-surface">
        <div className="px-4.5 py-3.5 border-b border-border flex items-center justify-between flex-wrap gap-3">
          <div>
            <h2 className="text-[14.5px] font-semibold m-0">Smart queue</h2>
            <div className="text-[12.5px] text-inkMuted mt-0.5">
              Ranked by effective score = urgency score + waiting-time bonus
            </div>
          </div>
          <div className="flex items-center gap-2.5">
            <div className="inline-flex border border-borderStrong rounded overflow-hidden">
              <button
                onClick={() => setCompare(false)}
                className={`px-3 py-1.5 text-[12.5px] border-r border-borderStrong ${!compare ? 'bg-brand text-white' : 'bg-white text-inkMuted'}`}
              >
                Smart queue
              </button>
              <button
                onClick={() => setCompare(true)}
                className={`px-3 py-1.5 text-[12.5px] ${compare ? 'bg-brand text-white' : 'bg-white text-inkMuted'}`}
              >
                Compare to FIFO
              </button>
            </div>
            <button
              onClick={() => advanceTime(15)}
              className="inline-flex items-center gap-1.5 border border-borderStrong rounded px-2.5 py-1.5 text-xs font-medium hover:bg-surfaceAlt"
            >
              <Icon name="bolt" size={13} /> Advance 15 min
            </button>
            {isDoctor && (
              <button
                onClick={handleCallNext}
                className="bg-brand hover:bg-brandDark text-white rounded px-2.5 py-1.5 text-xs font-medium"
              >
                Call next patient
              </button>
            )}
          </div>
        </div>
        <div>{compare ? <ComparePanel smart={smart} fifo={fifo} config={config} /> : <QueueTable patients={smart} config={config} action={action} />}</div>
      </div>

      {isDoctor && (
        <ConsultPanel
          patient={activePatient}
          notes={notes}
          setNotes={setNotes}
          config={config}
          onComplete={() => {
            if (activePatient) {
              completeConsult(activePatient.id, notes);
              setActivePatientId(null);
            }
          }}
        />
      )}
    </div>
  );
}

function ConsultPanel({ patient, notes, setNotes, config, onComplete }) {
  if (!patient) {
    return (
      <div className="border border-border rounded bg-surface">
        <div className="py-8 px-4.5 text-center text-inkMuted text-sm">
          Select &ldquo;Consult&rdquo; on a patient, or &ldquo;Call next patient&rdquo; to open a case here.
        </div>
      </div>
    );
  }
  const score = effectiveScore(patient, config);
  const level = levelOf(score);
  const doctor = DOCTORS.find((d) => d.id === patient.doctorId);

  return (
    <div className="border border-border rounded bg-surface">
      <div className="px-4.5 py-3.5 border-b border-border flex items-center justify-between">
        <div>
          <h2 className="text-[14.5px] font-semibold m-0">
            {patient.name}{' '}
            <span className="font-mono text-inkFaint font-normal text-[12.5px]">
              {patient.token} · {patient.id}
            </span>
          </h2>
          <div className="text-[12.5px] text-inkMuted mt-0.5">
            {patient.age} yrs · {patient.gender === 'F' ? 'Female' : 'Male'} · {patient.dept}
          </div>
        </div>
        <div className="flex items-center gap-2">
          <Pill level={level} />
          <span className="text-xs text-inkMuted font-mono tnum">score {score}</span>
        </div>
      </div>
      <div className="p-4.5 grid grid-cols-2 gap-4.5">
        <div>
          <div className="flex flex-col gap-1.5 mb-3.5">
            <label className="text-[12.5px] font-medium text-inkMuted">Consultation notes</label>
            <textarea
              value={notes}
              onChange={(e) => setNotes(e.target.value)}
              placeholder="Assessment, findings, prescription notes..."
              className="border border-borderStrong rounded px-2.5 py-2 min-h-[70px]"
            />
          </div>
          <button onClick={onComplete} className="bg-brand hover:bg-brandDark text-white rounded px-3.5 py-2.5 font-medium">
            Mark visit complete
          </button>
        </div>
        <div>
          <Row label="Symptom severity" value={`${patient.inputs.symptom} / ${config.symptomMax}`} />
          <Row label="Vital sign risk" value={`${patient.inputs.vital} / ${config.vitalMax}`} />
          <Row label="Reported pain" value={`${patient.inputs.pain} / 10`} />
          <Row label="Age/risk factor" value={`${patient.inputs.age} / ${config.ageMax}`} />
          <Row
            label="Existing risk factors"
            value={patient.inputs.risk.length ? patient.inputs.risk.map((k) => RISK_FACTORS.find((r) => r.key === k).label).join(', ') : 'None recorded'}
          />
          <Row label="Waiting-time bonus" value={`+${waitingBonus(patient.waitMinutes, config)}`} />
          <Row label="Effective score" value={score} />
        </div>
      </div>
    </div>
  );
}

function Row({ label, value }) {
  return (
    <div className="flex justify-between text-sm py-1.5 border-b border-dashed border-border last:border-none">
      <span className="text-inkMuted">{label}</span>
      <b className="font-mono tnum font-semibold text-right">{value}</b>
    </div>
  );
}
