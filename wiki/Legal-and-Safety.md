# Legal and Safety

**IMPORTANT**: Please read this entire document before using Flipagotcha.

## ⚠️ Legal Disclaimer

**READ THIS CAREFULLY BEFORE USING THIS SOFTWARE**

Flipagotcha is provided for **EDUCATIONAL AND AUTHORIZED SECURITY TESTING PURPOSES ONLY**.

### Your Responsibilities

By using this software, you agree:

1. **You are solely responsible** for your actions and their consequences
2. **You will only use** this software on networks you own or have explicit written permission to test
3. **You understand** that unauthorized use may be illegal in your jurisdiction
4. **You will comply** with all applicable laws and regulations
5. **You will not use** this software for malicious purposes

### No Warranty

This software is provided "AS IS" without warranty of any kind, express or implied, including but not limited to:
- Warranties of merchantability
- Fitness for a particular purpose
- Non-infringement

See the [LICENSE](../LICENSE) file for complete terms.

### Liability

The authors and contributors of this software:
- **Are not responsible** for any misuse of this software
- **Are not liable** for any damages or legal consequences resulting from its use
- **Do not condone** illegal or unethical use
- **Provide no support** for illegal activities

**YOU USE THIS SOFTWARE AT YOUR OWN RISK.**

---

## Legal Considerations by Jurisdiction

### United States

#### Computer Fraud and Abuse Act (CFAA)

Unauthorized access to computer systems is illegal under 18 U.S.C. § 1030.

**This includes:**
- Accessing WiFi networks without permission
- Interfering with network communications
- Capturing data from networks you don't own

**Penalties:**
- Criminal charges
- Fines up to $250,000
- Prison sentences up to 20 years (aggravated cases)
- Civil liability

#### Wiretap Act

18 U.S.C. § 2511 prohibits:
- Intercepting electronic communications
- Capturing network traffic without authorization

#### State Laws

Many states have additional computer crime laws with varying penalties.

### European Union

#### General Data Protection Regulation (GDPR)

Capturing personal data without consent may violate GDPR:
- WiFi packets may contain personal information
- Unauthorized capture = illegal processing
- Heavy fines possible (up to €20 million or 4% of revenue)

#### Computer Misuse Directives

EU member states have laws against:
- Unauthorized access to computer systems
- Network interference
- Data interception

### United Kingdom

#### Computer Misuse Act 1990

Illegal to:
- Access computer systems without authorization
- Modify computer material without authorization
- Facilitate computer misuse

**Penalties:**
- Fines
- Prison sentences up to 10 years

### Canada

#### Criminal Code

Section 342.1 prohibits:
- Unauthorized use of computer systems
- Interception of communications

### Australia

#### Cybercrime Act 2001

Prohibits:
- Unauthorized access to data
- Unauthorized modification of data
- Unauthorized impairment of electronic communications

### Other Countries

**Laws vary significantly.** Research your specific jurisdiction's laws regarding:
- Computer fraud
- Unauthorized access
- Network interference
- Data interception
- Wireless communication laws

---

## What is Legal Use?

### Authorized Testing Scenarios

✅ **Legal and Ethical:**

1. **Your Own Networks**
   - Testing your home WiFi
   - Securing your personal networks
   - Educational experiments on your own equipment

2. **Professional Penetration Testing**
   - Written contract with client
   - Defined scope of testing
   - Rules of engagement documented
   - Authorization from network owner

3. **Employer Authorization**
   - Written permission from IT department
   - Testing company networks as part of security role
   - Documented authorization

4. **Academic/Research Settings**
   - University lab environments
   - Controlled research networks
   - With institutional approval
   - IRB approval if involving human subjects

5. **Security Training**
   - Authorized training environments
   - Dedicated training networks
   - Supervised by instructors
   - No external targets

### Getting Proper Authorization

**For Professional Testing:**

1. **Written Contract**
   - Specifies networks to be tested
   - Defines testing methodology
   - Lists authorized activities
   - Time frame for testing
   - Reporting requirements

2. **Rules of Engagement**
   - What is allowed
   - What is prohibited
   - Emergency contacts
   - Stop conditions

3. **Keep Documentation**
   - Authorization letter
   - Contract
   - Scope documents
   - Communications

**Example Authorization Letter:**

```
To: [Tester Name]
From: [Network Owner]
Date: [Date]
Subject: Authorization for Network Security Testing

I, [Name], as [Title] of [Organization], hereby authorize
[Tester Name] to perform security testing on the following
networks:

Network SSIDs:
- [Network 1]
- [Network 2]

Testing Period: [Start Date] to [End Date]

Authorized Activities:
- WiFi network scanning
- Packet capture
- Handshake capture
- [Other specific activities]

This authorization is limited to the networks listed above
and the specified time period.

Signature: _________________
Date: _____________________
```

---

## What is NOT Legal

### ❌ Illegal Activities

**Never do these without authorization:**

1. **Scanning Networks You Don't Own**
   - Neighbor's WiFi
   - Public WiFi (without permission)
   - Business networks
   - Government networks

2. **Deauthentication Attacks**
   - Disconnecting users from networks
   - Denial of service attacks
   - Interference with communications

3. **Capturing Other People's Data**
   - Packet sniffing on networks you don't control
   - Intercepting communications
   - Collecting personal information

4. **Unauthorized Access**
   - Using captured handshakes to crack passwords
   - Accessing networks without permission
   - Bypassing security measures

5. **Distributing Malicious Tools**
   - Providing tools specifically for illegal use
   - Encouraging illegal activity
   - Facilitating unauthorized access

---

## Ethical Considerations

Beyond legal requirements, consider ethics:

### Responsible Disclosure

If you discover vulnerabilities:
1. **Don't publicize immediately**
2. **Contact the affected party** privately
3. **Give time to fix** before disclosure
4. **Be helpful**, not destructive

### Privacy

Respect privacy:
- Don't share captured data publicly
- Don't snoop on others' communications
- Treat data you capture as confidential
- Delete data when testing complete

### Intent Matters

Ask yourself:
- Why am I doing this?
- Who could be harmed?
- Is this the right thing to do?
- How would I feel if someone did this to me?

### Professional Standards

Security professionals follow:
- **EC-Council Code of Ethics**
- **ISC² Code of Ethics**
- **SANS Institute Ethics Guidelines**
- **Local professional standards**

---

## Safety Considerations

### Physical Safety

#### Device Safety

**Flipper Zero:**
- Use correct GPIO pins
- Don't short circuit pins
- Follow voltage requirements (3.3V)

**ESP32:**
- Ensure proper power supply
- Don't overheat
- Use quality USB cables
- Proper ventilation

#### Electrical Safety

- No water near electronics
- Proper insulation
- Avoid static discharge
- Don't modify hardware unless experienced

### Operational Safety

#### Battery Safety

- Monitor battery levels
- Don't over-discharge
- Use genuine batteries
- Proper charging practices

#### RF Safety

- WiFi exposure generally safe
- Follow FCC/local RF guidelines
- Don't transmit continuously for extended periods

#### Data Safety

**Protect Captured Data:**
- Encrypt storage
- Secure deletion when done
- No cloud backup without encryption
- Physical security of devices

---

## Jurisdictional Warnings

### High-Risk Locations

Exercise extreme caution in:

#### Government Facilities
- Military installations
- Government buildings
- Airports (especially near aircraft)
- Border crossings

#### Critical Infrastructure
- Power plants
- Water treatment facilities
- Telecommunications facilities
- Transportation hubs

#### Educational Institutions
- Schools and universities (without permission)
- May have specific policies
- Could trigger security responses

#### Private Property
- Businesses
- Residential areas
- Private networks

### International Travel

**DO NOT:**
- Use this tool near borders
- Carry to countries with strict laws
- Use in airports or on aircraft
- Use without researching local laws

**Be aware:**
- Laws vary dramatically
- Some countries have strict penalties
- Ignorance is not a defense
- Can result in detention or deportation

---

## Reporting Illegal Activity

If you witness illegal use:

1. **Don't confront** the person
2. **Report to authorities:**
   - Local law enforcement
   - FBI Cyber Division (US): ic3.gov
   - National Cyber Security Centre (UK)
   - Local equivalent

3. **Document what you saw** (safely)
4. **Don't participate** in illegal activity

---

## Additional Resources

### Legal Research

- [Electronic Frontier Foundation](https://www.eff.org/)
- [OWASP Legal Project](https://owasp.org/www-project-legal/)
- Local cyber law resources

### Professional Organizations

- [EC-Council](https://www.eccouncil.org/)
- [ISC²](https://www.isc2.org/)
- [SANS Institute](https://www.sans.org/)
- [OWASP](https://owasp.org/)

### Security Standards

- NIST Cybersecurity Framework
- ISO 27001
- PCI DSS (for payment systems)
- HIPAA (for healthcare)

---

## When in Doubt

If you're unsure whether something is legal:

1. **Don't do it**
2. **Consult a lawyer** specializing in cyber law
3. **Get written authorization**
4. **Research your jurisdiction's laws**
5. **Ask yourself**: "Would I want someone doing this to my network?"

---

## Summary

### ✅ DO:
- Use on your own networks
- Get written authorization
- Follow the law
- Be ethical
- Document everything
- Learn responsibly

### ❌ DON'T:
- Scan unauthorized networks
- Attack networks
- Capture others' data
- Break the law
- Ignore this guidance
- Use maliciously

---

## Acceptance

**By using Flipagotcha, you acknowledge:**

1. You have read and understood this document
2. You agree to use this software legally and ethically
3. You accept full responsibility for your actions
4. You understand the potential legal consequences
5. You will seek appropriate authorization before testing

**If you cannot agree to these terms, DO NOT USE THIS SOFTWARE.**

---

## Updates

This document may be updated periodically. Check for the latest version at:
- [GitHub Repository](https://github.com/1090mb/flipagotcha)
- [Wiki Legal Page](Legal-and-Safety.md)

**Last Updated**: February 2026

---

## Contact

For legal concerns or questions:
- Open a [GitHub Issue](https://github.com/1090mb/flipagotcha/issues)
- Consult your legal counsel
- Contact appropriate authorities if needed

**Remember: When in doubt, don't. Better to ask permission than forgiveness when dealing with computer security laws.**
