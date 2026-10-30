
# Frequently Asked Questions about PSA API compliance


## Introduction

The Platform Security Architecture initiative introduced a set of common APIs for accessing Root of Trust services. These APIs provide standardized interfaces for cryptography, secure storage, attestation, and firmware update.

Crypto vendors may also be interested in adopting just the Crypto API part for their crypto libraries or hardware.

PSA Certified currently reviews PSA API compliance evidence, issues API compliance badges, and lists compliant products. Its badges cover both device-level vendors implementing a set of PSA APIs and crypto vendors implementing all or a subset of the Crypto API. The test suite is named the **PSA API Compliance tests** and is maintained in [GlobalPlatform/psa-api-compliance](https://github.com/GlobalPlatform/psa-api-compliance).

## How should those APIs be referred to?

The official name for those APIs is `PSA APIs`: the PSA Crypto API, PSA Secure Storage API, PSA Attestation API, and PSA Firmware Update API. Specification sources and reference headers are maintained in [GlobalPlatform/psa-api](https://github.com/GlobalPlatform/psa-api/). Published specifications remain on the [PSA API specifications website](https://arm-software.github.io/psa-api/).
Another common way to refer to those APIs is `PSA functional APIs`.

`PSA Crypto API` is often shortened to `Crypto API` in documentation.

The evaluation program retains the name **PSA Certified**. It issues API compliance badges and Crypto API compliance badges.

## What is PSA API compliance? How does it relate to PSA Certified security evaluation?

[PSA Certified](https://www.psacertified.org) security evaluation assesses the security properties and robustness of products and their Roots of Trust.

PSA Certified security evaluation does not mandate the implementation of the PSA APIs. Root of Trust implementers are free to choose the interfaces they offer to their users; security evaluation looks at the security properties and robustness offered by implementations.

The PSA API Compliance tests check specified API behaviors and provide evidence of compliance. PSA Certified reviews test results and awards API compliance badges. An implementation does not need a PSA Certified security evaluation at any level to receive an API compliance badge.

You can obtain a PSA Certified security evaluation without implementing PSA APIs, and you can implement PSA APIs and seek a compliance badge without applying for a security evaluation.

## Have the PSA APIs themselves been certified by PSA Certified?

No. PSA Certified security evaluation assesses implementations, rather than the API specifications.

Using standardized APIs for Root of Trust services can help in deployment and adoption of secure devices. The PSA APIs were developed as part of the original Platform Security Architecture (PSA) initiative.

GlobalPlatform now governs the PSA Certified program, the PSA APIs, and the PSA API Compliance test suite.

## What is the PSA API compliance badge?

The general API compliance badge is meant for chip vendors, system software vendors, and device manufacturers who provide access to Root of Trust services through the PSA APIs.

For device vendors, obtaining the API compliance badge is a way to communicate to their customers that they can rely on the presence of standardized APIs to access Root of Trust services. The [PSA Certified program](https://www.psacertified.org/getting-certified/functional-api-certification/) requires Crypto and at least one Secure Storage service; Attestation and Firmware Update are optional. The suite provides tests for:

- Crypto APIs, with the enabled algorithms and features configured for the implementation under test.
- At least one Secure Storage service: ITS (Internal Trusted Storage) or PS (Protected Storage), or both.
- The Attestation service API, if implemented.

Confirm the required scope and accepted test revision with PSA Certified before applying.

Once you get your badge, your product can be listed on [PSA Certified products](https://products.psacertified.org/).

## What is the PSA Crypto API compliance badge?

The Crypto API compliance badge is meant for pure crypto players who provide access to their crypto software or hardware through the PSA Crypto API.

Product examples include open-source and commercial crypto libraries, crypto hardware accelerators, secure elements, smart cards, TPM equivalents, TrustZone-based services, and anything that offers high-level cryptography services like hash, signature, or encryption.

For pure crypto players, obtaining the Crypto API compliance badge is a way to communicate to their customers:

- Compliance with the Crypto API standard, implying no need for developers to re-learn yet another proprietary API.
- How many high-level crypto services are supported by a given crypto product.

Ideally, it should be possible for developers to determine at a glance whether a crypto engine (hardware or software) has enough functionality to implement their use cases. This is exactly what the Crypto API compliance badge is trying to achieve.


## Which high-level crypto services are included in the Crypto API compliance badge?

High-level crypto services describe the main operations offered by a crypto engine (hardware or software) like encrypt/decrypt, sign/verify, hash, generate random, and key derivation and agreement protocols. Most crypto use cases can be described by combining those high-level functions.

Announcing high-level crypto services is useful for developers looking for ways to develop cryptography-based use cases like secure network connections, secure storage, or authentication protocols. For example, establishing a handshake in a mutually-authenticated protocol may require key agreement, key generation, symmetric ciphers, and hash functions.


High-level Crypto services  | Crypto badge mention
--------------------------  | --------------------
Hash                        | `crypto_hash`
MAC                         | `crypto_mac`
Symmetric and stream ciphers            | `crypto_cipher`
Asymmetric signature and verification   | `crypto_asym_sign`
Asymmetric encryption/decryption        | `crypto_asym_crypt`
Authenticated Encryption with Associated Data   | `crypto_aead`
Key derivation              | `crypto_derivation`
Key agreement               | `crypto_agreement`
Random Number Generation    | `crypto_rng`
PAKE algorithm support      | `crypto_pake`

Functions associated with each functional domain are listed below. The `psa_` prefix is omitted, and key attribute accessors are grouped. The supported subset is recorded in the compliance evidence.

| Badge mention              | Functions
| -------------------------  | -----
| All badges                 | `crypto_init`<br>`import_key`<br>`export_key`<br>`destroy_key`<br>key attribute setters/getters<br>`generate_key`<br>`copy_key`
| `crypto_hash` | `hash_setup`<br>`hash_update`<br>`hash_verify`<br>`hash_finish`<br>`hash_abort`<br><br>optional:<br>`hash_clone`<br>`hash_compute`<br>`hash_compare`<br>`hash_suspend`<br>`hash_resume`
| `crypto_mac`  | `mac_sign_setup`<br>`mac_update`<br>`mac_sign_finish`<br>`mac_verify_setup`<br>`mac_verify_finish`<br>`mac_abort`<br><br>optional:<br>`mac_compute`<br>`mac_verify`
| `crypto_asym_sign` | `sign_hash`<br>`verify_hash`<br>`sign_message`<br>`verify_message`
| `crypto_asym_crypt`  | `asymmetric_encrypt`<br>`asymmetric_decrypt`
| `crypto_cipher` | `cipher_encrypt`<br>`cipher_decrypt`<br>`cipher_encrypt_setup`<br>`cipher_decrypt_setup`<br>`cipher_generate_iv`<br>`cipher_set_iv`<br>`cipher_update`<br>`cipher_finish`<br>`cipher_abort`
| `crypto_aead` | `aead_encrypt`<br>`aead_decrypt`<br><br>optional:<br>`aead_encrypt_setup`<br>`aead_decrypt_setup`<br>`aead_generate_nonce`<br>`aead_set_nonce`<br>`aead_set_lengths`<br>`aead_update_ad`<br>`aead_update`<br>`aead_finish`<br>`aead_abort`<br>`aead_verify`
| `crypto_derivation`  | `key_derivation_setup`<br>`key_derivation_input_bytes`<br>`key_derivation_input_integer`<br>`key_derivation_input_key`<br>`key_derivation_key_agreement`<br>`key_derivation_output_bytes`<br>`key_derivation_output_key`<br>`key_derivation_abort`<br>`key_derivation_set_capacity`<br>`key_derivation_get_capacity`<br>`key_derivation_verify_bytes`<br>`key_derivation_verify_key`
| `crypto_agreement`   | `raw_key_agreement`
| `crypto_rng`         | `generate_random`
| `crypto_pake`        | A consistent set of PAKE operations needed to complete a PAKE key exchange.

This list is probably not exhaustive and offers flexibility. Crypto vendors are expected to use common sense to determine which functions of a given subset are interesting for their own users. For example, it is not necessary to implement all hash-related functions to be in a position to claim a `crypto_hash` badge. Asymmetric crypto signature could support signing hashes only and not entire messages, and still claim a `crypto_asym_sign` badge. Those badges are not strict grades, they are a way for vendors to summarize to their customers what their products support, inviting them to review specific data sheets for more detailed information.

## What about crypto primitives like AES, RSA, SHA?

The API compliance badge only shows what kind of high-level crypto services are offered by compatible products. Low-level crypto primitives like AES, RSA, SHA, or specific elliptic curves are already always clearly announced by crypto vendors on their product datasheets. The API compliance badge is about functional compliance with a promise of portability, rather than about low-level crypto primitive support.

The Crypto API part of the compliance kit expects a minimal set of crypto primitives to be present to pass test cases. See test cases for more details about which primitives are tested.

## What is the process to obtain a PSA API compliance badge?

To pass general PSA API compliance tests you need to:

- Download the compliance kit from this repository.
- Possibly adapt the test suite to reflect which tests you want to pass or skip.
- Run the tests on a device that runs the Root of Trust you want to apply for.
- Submit the logs and product information using the current process published by [PSA Certified](https://www.psacertified.org/getting-certified/functional-api-certification/), identifying the APIs and features tested and explaining any skipped or excluded tests. Confirm the accepted test revision and submission arrangements with [PSA Certified](https://www.psacertified.org/what-is-psa-certified/contact/) before applying. Attestation is optional, and the badge records the scope of compliance.

To pass Crypto API compliance tests you need to:

- Download the compliance kit from this repository.
- Possibly adapt the test suite to reflect which tests you want to pass or skip.
- Run the tests on any compatible platform.
- Submit the logs and product information to PSA Certified using the process above, and mention you want to apply for Crypto API compliance.

It is possible to apply for Crypto API compliance even when implementing very restricted subsets of Crypto. For example, a hardware hash accelerator that offers compatible hash APIs could apply to PSA Certified for a Crypto API compliance badge that would only mention `crypto_hash`.

## How much does it cost to obtain a PSA API compliance badge?

Confirm current fees and terms with [PSA Certified](https://www.psacertified.org/what-is-psa-certified/contact/).

## How long does it take to obtain a PSA API compliance badge?

PSA Certified reviews the submitted evidence. Contact [PSA Certified](https://www.psacertified.org/what-is-psa-certified/contact/) for the current turnaround time.

--------------

*Copyright (c) 2023-2024, 2026, Arm Limited and Contributors. All rights reserved.*
