'use client';

import DashboardShell from '@/components/DashboardShell';
import { useApp } from '@/context/AppProvider';
import { DOCTORS } from '@/lib/mockData';
import { effectiveScore, levelOf, sortedByPriority, fmtWait, fmtTime } from '@/lib/engine';
import { Stat, Pill, Banner, Steps, KV } from '@/components/Bits';
import { Icon } from '@/components/Icons';

export default function PatientPage() {
  return (
    <DashboardShell role="patient">
      <PatientBody />
    </DashboardShell>
  );
}

const STEPS = ['Scheduled', 'Checked-in', 'Waiting', 'In consultation', 'Completed'];

function PatientBody() {
  const { queue, config, myPatientId } = useApp();
  const patient = queue.find((p) => p.id === myPatientId) || queue[0];
  const waiting = queue.filter((p) => p.status === 'waiting');
  const smart = sortedByPriority(waiting, config);
  const pos = smart.findIndex((x) => x.id === patient.id);
  const level = levelOf(effectiveScore(patient, config));
  const stepIndex = patient.status === 'completed' ? 4 : patient.status === 'in-treatment' ? 3 : 2;
  const doctor = DOCTORS.find((d) => d.id === patient.doctorId);

  return (
    <div className="grid grid-cols-2 gap-4.5">
      <div className="flex flex-col gap-4.5">
        <div className="border border-border rounded bg-surface">
          <div className="px-4.5 py-3.5 border-b border-border flex items-center justify-between">
            <div>
              <h2 className="text-[14.5px] font-semibold m-0">{patient.name}</h2>
              <div className="text-[12.5px] text-inkMuted mt-0.5">
                {patient.dept} · with {doctor?.name}
              </div>
            </div>
            <span className="text-inkMuted font-mono text-xs bg-surfaceAlt rounded-full px-2.5 py-1">{patient.token}</span>
          </div>
          <div className="p-4.5">
            <Steps items={STEPS} currentIndex={stepIndex} />
          </div>
        </div>

        <div className="border border-border rounded bg-surface">
          <div className="px-4.5 py-3.5 border-b border-border">
            <h2 className="text-[14.5px] font-semibold m-0">Queue status</h2>
          </div>
          <div className="p-4.5">
            {patient.status === 'waiting' ? (
              <>
                <div className="grid grid-cols-2 gap-3.5">
                  <Stat value={pos >= 0 ? pos + 1 : '—'} label="Position in queue" />
                  <Stat value={fmtWait(patient.waitMinutes)} label="Time waited so far" />
                </div>
                <div className="mt-3.5">
                  <Banner>
                    <Icon name="info" className="text-brand shrink-0 mt-px" />
                    <span>
                      Your position can move if a more urgent case arrives, or if you&rsquo;ve been waiting long enough
                      that the fairness bonus lifts your place in line.
                    </span>
                  </Banner>
                </div>
              </>
            ) : patient.status === 'in-treatment' ? (
              <Banner>
                <Icon name="bolt" className="text-brand shrink-0 mt-px" />
                <span>You&rsquo;ve been called through to {doctor?.name}.</span>
              </Banner>
            ) : (
              <Banner>
                <Icon name="info" className="text-brand shrink-0 mt-px" />
                <span>This visit is marked complete. Notes have been added to your record.</span>
              </Banner>
            )}
          </div>
        </div>
      </div>

      <div className="flex flex-col gap-4.5">
        <div className="border border-border rounded bg-surface">
          <div className="px-4.5 py-3.5 border-b border-border">
            <h2 className="text-[14.5px] font-semibold m-0">Appointment</h2>
          </div>
          <div className="p-4.5">
            <KV label="Token" value={patient.token} />
            <KV label="Department" value={patient.dept} />
            <KV label="Doctor" value={doctor?.name} />
            <KV label="Checked in" value={fmtTime(patient.arrival)} />
            <div className="flex justify-between text-sm py-1.5">
              <span className="text-inkMuted">Priority level</span>
              <Pill level={level} />
            </div>
          </div>
        </div>
        <div className="border border-border rounded bg-surface">
          <div className="px-4.5 py-3.5 border-b border-border">
            <h2 className="text-[14.5px] font-semibold m-0">Visit history</h2>
          </div>
          <div>
            <div className="px-4.5 py-3 border-b border-border text-[13px]">
              <b>General Medicine</b>
              <div className="text-inkMuted text-xs mt-0.5">14 Apr — Routine follow-up, Dr. R. Iyer</div>
            </div>
            <div className="px-4.5 py-3 text-[13px]">
              <b>General Medicine</b>
              <div className="text-inkMuted text-xs mt-0.5">02 Feb — Prenatal check, Dr. R. Iyer</div>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}
