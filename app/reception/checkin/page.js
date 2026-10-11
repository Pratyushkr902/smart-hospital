'use client';

import { useRouter } from 'next/navigation';
import DashboardShell from '@/components/DashboardShell';
import { Banner, Pill, Stat } from '@/components/Bits';
import { Icon } from '@/components/Icons';
import { useApp } from '@/context/AppProvider';
import { DEPARTMENTS, DOCTORS, RISK_FACTORS } from '@/lib/mockData';
import { effectiveScore, levelOf } from '@/lib/engine';

export default function CheckinPage() {
  return (
    <DashboardShell role="reception">
      <CheckinBody />
    </DashboardShell>
  );
}

function CheckinBody() {
  const router = useRouter();
  const { queue, config, checkInPatient } = useApp();

  function handleSubmit(e) {
    e.preventDefault();
    const fd = new FormData(e.currentTarget);
    const age = Number(fd.get('age'));
    checkInPatient({
      name: fd.get('name'),
      age,
      gender: fd.get('gender'),
      dept: fd.get('dept'),
      doctorId: fd.get('doctorId'),
      inputs: {
        symptom: Number(fd.get('symptom')),
        vital: Number(fd.get('vital')),
        pain: Number(fd.get('pain')),
        age: age >= 65 ? 8 : age <= 5 ? 6 : 2,
        risk: fd.getAll('risk'),
      },
    });
    router.push('/reception/queue');
  }

  const waiting = queue.filter((p) => p.status === 'waiting').length;
  const inTreatment = queue.filter((p) => p.status === 'in-treatment').length;
  const completed = queue.filter((p) => p.status === 'completed').length;

  return (
    <div className="grid grid-cols-[1.5fr_1fr] gap-4.5 gap-x-[18px] gap-y-[18px]">
      <div className="border border-border rounded bg-surface">
        <div className="px-4.5 py-3.5 border-b border-border">
          <h2 className="text-[14.5px] font-semibold m-0">Register &amp; check in</h2>
          <div className="text-[12.5px] text-inkMuted mt-0.5">
            Symptom and vital inputs feed the priority engine — they are not a diagnosis.
          </div>
        </div>
        <div className="p-4.5">
          <form onSubmit={handleSubmit} className="space-y-0">
            <div className="grid grid-cols-2 gap-3">
              <Field label="Full name">
                <input name="name" placeholder="e.g. Rohan Kapoor" required className="input" />
              </Field>
              <Field label="Age">
                <input name="age" type="number" min="0" max="110" defaultValue="30" required className="input" />
              </Field>
            </div>
            <div className="grid grid-cols-2 gap-3">
              <Field label="Gender">
                <select name="gender" className="input">
                  <option value="F">Female</option>
                  <option value="M">Male</option>
                  <option value="O">Other</option>
                </select>
              </Field>
              <Field label="Department">
                <select name="dept" className="input">
                  {DEPARTMENTS.map((d) => (
                    <option key={d}>{d}</option>
                  ))}
                </select>
              </Field>
            </div>
            <Field label="Attending doctor">
              <select name="doctorId" className="input">
                {DOCTORS.map((d) => (
                  <option key={d.id} value={d.id}>
                    {d.name} — {d.dept}
                  </option>
                ))}
              </select>
            </Field>

            <div className="h-px bg-border my-4.5" />

            <div className="grid grid-cols-2 gap-3">
              <Field label="Symptom severity">
                <select name="symptom" defaultValue="22" className="input">
                  <option value="4">Mild</option>
                  <option value="14">Moderate</option>
                  <option value="22">Significant</option>
                  <option value="30">Severe</option>
                </select>
              </Field>
              <Field label="Vital sign risk">
                <select name="vital" defaultValue="18" className="input">
                  <option value="2">Normal range</option>
                  <option value="10">Slightly abnormal</option>
                  <option value="18">Elevated</option>
                  <option value="25">Critical range</option>
                </select>
              </Field>
            </div>

            <PainField />

            <Field label="Existing risk factors">
              <div className="flex flex-wrap gap-x-4 gap-y-2 mt-0.5">
                {RISK_FACTORS.map((r) => (
                  <label key={r.key} className="text-[12.5px] flex items-center gap-1.5">
                    <input type="checkbox" name="risk" value={r.key} />
                    {r.label}
                  </label>
                ))}
              </div>
            </Field>

            <div className="my-4">
              <Banner>
                <Icon name="info" className="text-brand shrink-0 mt-px" />
                <span>Scores are a queue-management aid for staff. Final clinical priority is always set by the attending doctor.</span>
              </Banner>
            </div>

            <button type="submit" className="bg-brand hover:bg-brandDark text-white rounded px-3.5 py-2.5 font-medium">
              Check in and assign token
            </button>
          </form>
        </div>
      </div>

      <div className="flex flex-col gap-4.5">
        <div className="border border-border rounded bg-surface">
          <div className="px-4.5 py-3.5 border-b border-border">
            <h2 className="text-[14.5px] font-semibold m-0">Today</h2>
          </div>
          <div className="p-4.5 grid grid-cols-2 gap-3.5">
            <Stat value={queue.length} label="Total registered" />
            <Stat value={waiting} label="Currently waiting" />
            <Stat value={inTreatment} label="In consultation" />
            <Stat value={completed} label="Completed" />
          </div>
        </div>
        <div className="border border-border rounded bg-surface">
          <div className="px-4.5 py-3.5 border-b border-border">
            <h2 className="text-[14.5px] font-semibold m-0">Just checked in</h2>
          </div>
          <div>
            {queue.slice(0, 4).map((p) => (
              <div key={p.id} className="flex justify-between px-4.5 py-2.5 border-b border-border last:border-none text-[13px]">
                <span>
                  {p.name}
                  <span className="font-mono text-inkFaint ml-2">{p.token}</span>
                </span>
                <Pill level={levelOf(effectiveScore(p, config))} />
              </div>
            ))}
          </div>
        </div>
      </div>

      <style jsx global>{`
        .input {
          border: 1px solid #c4cdd0;
          border-radius: 3px;
          padding: 8px 10px;
          background: #fff;
          width: 100%;
        }
      `}</style>
    </div>
  );
}

function Field({ label, children }) {
  return (
    <div className="flex flex-col gap-1.5 mb-3.5">
      <label className="text-[12.5px] font-medium text-inkMuted">{label}</label>
      {children}
    </div>
  );
}

function PainField() {
  return (
    <div className="flex flex-col gap-1.5 mb-3.5">
      <label className="text-[12.5px] font-medium text-inkMuted">Reported pain (0–10 scale)</label>
      <input
        type="range"
        name="pain"
        min="0"
        max="10"
        defaultValue="5"
        onInput={(e) => {
          const out = e.currentTarget.nextElementSibling;
          if (out) out.textContent = e.currentTarget.value;
        }}
      />
      <div className="text-[11.5px] text-inkFaint font-mono">5</div>
    </div>
  );
}
