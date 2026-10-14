# Smart Hospital — Frontend (Next.js + Tailwind)

Frontend for the Smart Hospital MVP, built to the stack in the PRD:
React / Next.js (App Router) + Tailwind CSS, running entirely on in-memory
mock data — no backend calls. The priority-scoring logic in `lib/engine.js`
mirrors the C++ engine spec in PRD §7 so it can be ported into
`PriorityEngine` / `ScoreCalculator` as-is once the Crow backend is wired up.

## Run it

```bash
npm install
npm run dev
```

Then open http://localhost:3000.

## Structure

```
app/                    routes (App Router)
  page.js               login / role picker
  reception/checkin      reception/queue
  doctor/queue
  admin/overview  admin/config  admin/log
  patient/
components/             shared UI (shell, queue table, icon set, small pieces)
context/AppProvider.js  in-memory store: queue, audit log, scoring config
lib/engine.js           pure priority-scoring functions (score, sort, FIFO compare)
lib/mockData.js         doctors, departments, risk factors, seed patients
```

## Notes

- No real authentication — the login screen just picks which dashboard you land on.
- State lives in React context and resets on full page reload, since there's
  no backend yet. Swapping in the real REST API means replacing the functions
  in `AppProvider.js` with `fetch` calls against the endpoints in PRD §11.
- Scoring weights on **Admin → Scoring config** are live — they recompute
  every patient's effective score immediately.
