#### I would actually use a three-stage process

For your application, I'd do this:

**Audit #1 — Fresh conversation**

Give it:

1. The complete repository.
2. Your security-audit prompt.
3. Minimal architectural context that is necessary to understand the application.

Ask it to produce the complete audit and issue files.

**Audit #2 — Completely fresh conversation**

Give another LLM conversation:

1. The same source code.
2. The same audit prompt.
3. **Do not give it Audit #1.**
4. Ask for an independent audit.

Ideally use a different model as well, if available. Different models have different blind spots.

Then compare the results.

**Audit #3 — Adversarial synthesis**

This is where the previous audits become useful.

Give the LLM:

- the source code;
- Audit #1;
- Audit #2;
- the issue lists from both.

Then tell it:

> Treat both audits as potentially wrong. Re-examine the source code yourself. Determine which findings are valid, which are false positives, which are duplicates, and which important vulnerabilities both auditors missed.

That third pass is much more valuable than simply saying "audit this again."



---



You are acting as a principal software security engineer and senior code reviewer.

Your task is to perform a comprehensive security, correctness, and reliability audit of this application.

## Application

The application consists of:

- HTML/CSS frontend
- JavaScript frontend logic
- Rust backend
- REST API
- Multi-user architecture
- Each user authenticates using an API key.
- Every API request is associated with one user's data only.

Assume this application will be exposed on the public Internet.

Do NOT assume anything is implemented correctly simply because it looks reasonable.

Challenge every assumption.

----------------------------------------

Audit Goals
----------------------------------------

Review the entire codebase for:

### Security

Authentication

Authorization

Privilege escalation

Horizontal privilege escalation

Vertical privilege escalation

API key misuse

API key leakage

Broken object-level authorization (BOLA)

Broken function-level authorization

Injection attacks

XSS

Stored XSS

Reflected XSS

DOM XSS

SQL injection

Command injection

Path traversal

Directory traversal

Unsafe file access

Unsafe deserialization

Template injection

CSRF (if applicable)

CORS configuration

Rate limiting

Brute force protection

Replay attacks

Session handling

Secrets management

Logging of sensitive data

Information disclosure

Timing attacks

Race conditions

Dencryption/key management issues

Input validation

Output encoding

Unsafe regex

Denial of Service risks

Resource exhaustion

Memory safety

Unsafe Rust usage

panic! handling

unwrap()/expect() misuse

Error handling that leaks information

Third-party dependency risks

Supply-chain risks

OWASP Top 10

OWASP API Top 10

----------------------------------------

Correctness
----------------------------------------

Find:

logic bugs

edge cases

race conditions

integer overflow

underflow

off-by-one errors

state inconsistencies

async bugs

deadlocks

improper locking

incorrect lifetime assumptions

error propagation mistakes

data corruption risks

serialization bugs

API contract mismatches

----------------------------------------

Frontend Review
----------------------------------------

Review all JavaScript for:

unsafe DOM manipulation

innerHTML misuse

unsafe eval

dynamic script loading

token leakage

localStorage/sessionStorage misuse

API key exposure

browser caching issues

CSP weaknesses

event handling bugs

async race conditions

----------------------------------------

Rust Review
----------------------------------------

Review for:

unsafe blocks

ownership mistakes

Arc/Mutex misuse

RwLock misuse

Send/Sync issues

Tokio async mistakes

blocking calls inside async

unwrap()/expect()

panic paths

Result handling

file permissions

path handling

serialization/deserialization

error handling

resource cleanup

----------------------------------------

API Review
----------------------------------------

Verify that:

Every endpoint authenticates correctly.

Every endpoint authorizes correctly.

Users cannot access another user's data.

User IDs supplied by clients are never trusted.

Server derives identity from authenticated credentials.

API keys cannot be enumerated.

Object IDs cannot be guessed.

Pagination is safe.

Filtering cannot expose unauthorized data.

Mass assignment vulnerabilities do not exist.

----------------------------------------

Threat Modeling
----------------------------------------

Assume an attacker:

has a valid API key

controls browser requests

can modify all HTTP requests

can replay requests

can intercept their own traffic

can fuzz endpoints

can send malformed JSON

can upload arbitrary files if uploads exist

tries privilege escalation

tries ID enumeration

tries resource exhaustion

tries timing attacks

tries injection attacks

tries malformed Unicode

tries oversized payloads

----------------------------------------

Review Method

For every issue found provide:

1. Severity
   Critical
   High
   Medium
   Low

2. Confidence
   High
   Medium
   Low

3. Location
   File
   Function
   Line(s)

4. Explanation

5. Attack scenario

6. Impact

7. Recommended fix

8. Example corrected code

Do NOT merely identify problems.

Show exactly why they are problems.

----------------------------------------

Output

Produce:

## Executive Summary

Overall security rating (1-10)

Top five risks

Most likely attack vectors

Most severe vulnerabilities

----------------------------------------

Then produce:

## Detailed Findings

One section per issue.

----------------------------------------

Finally produce:

## Positive Findings

Identify places where the code follows good security practices.

----------------------------------------

If you are uncertain about a potential vulnerability:

State your assumptions.

Do not invent evidence.

Mark the issue as "Requires Verification."

Do not skip difficult analysis.

After completing the audit, perform a second pass assuming the first pass missed subtle vulnerabilities.

Specifically search for:

- privilege escalation
- authentication bypass
- authorization bypass
- race conditions
- business logic flaws
- data leakage
- missing validation
- TOCTOU bugs
- concurrency issues
- inconsistent assumptions between frontend and backend

Only report newly discovered issues.

-------------------

Generate a report of your analysis and a set of issues, if there are any, that I can enter into my issue tracker. Use the standard issue template, one markdown file for each issue.

I will not be available for questions once you begin, so after I tell you to proceed, carry through to the end without any additional questions.

Do you have any questions before you begin?
