<script lang="ts">
	import '../app.postcss';
	import { AppShell, AppBar } from '@skeletonlabs/skeleton';
</script>

<!-- App Shell -->
<AppShell>
	<svelte:fragment slot="header">
		<!-- App Bar -->
		<AppBar>
			<svelte:fragment slot="lead">
				<strong class="text-xl uppercase">Driverbot Controlpanel</strong> <!-- Titel och design på header i hemsidan.-->
			</svelte:fragment>
			<svelte:fragment slot="trail">
				<!-- Länk till github repot -->
				<a
					class="btn btn-sm variant-ghost-surface"
					href="https://github.com/abbindustrigymnasium/driverbot-liam.git"
					target="_blank"
					rel="noreferrer"
				>
					GitHub
				</a>
			</svelte:fragment>
		</AppBar>
	</svelte:fragment>
	<!-- Page Route Content -->
	<slot />
</AppShell>
